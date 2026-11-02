#include "QueueEngine.h"
#include "QRCodeGenerator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>

QueueEngine::QueueEngine(const std::string& dataDir)
    : nextNumericId(101),
      gracePeriodMinutes(5),
      autoWorkloadBalancing(true),
      qrCodeTargetUrl("http://172.17.134.16:3000/web_gui/index.html?tab=studentKiosk"),
      dataDirectory(dataDir)
{
    initializeDefaultCollegeServices();
    initializeDefaultCounters();
    loadState();
}

QueueEngine::~QueueEngine() {
    saveState();
}

void QueueEngine::initializeDefaultCollegeServices() {
    services["ADM"] = ServiceType("ADM", "Admission & Registration",
                                  "Document verification, branch allocation, enrollment", "#2563eb", 8.0);
    services["FEE"] = ServiceType("FEE", "Fees & Accounts",
                                  "Tuition fee deposit, scholarship receipts, challans", "#059669", 4.0);
    services["EXM"] = ServiceType("EXM", "Examination & Results",
                                  "Hall tickets, revaluation, grade card queries", "#d97706", 6.0);
    services["CRT"] = ServiceType("CRT", "Transcripts & Certificates",
                                  "Leaving certificates, bona fide, official transcripts", "#7c3aed", 5.0);
    services["GEN"] = ServiceType("GEN", "General Enquiry",
                                  "College campus guidance, office timings, general queries", "#0891b2", 3.0);

    // Initialize an empty FIFO queue for each service
    for (std::map<std::string, ServiceType>::iterator it = services.begin(); it != services.end(); ++it) {
        if (queues.find(it->first) == queues.end()) {
            queues[it->first] = ServiceQueue();
        }
    }
}

void QueueEngine::initializeDefaultCounters() {
    counters.clear();

    std::vector<std::string> c1Svcs;
    c1Svcs.push_back("ADM");
    c1Svcs.push_back("CRT");
    c1Svcs.push_back("GEN");
    counters.push_back(Counter(1, "Counter 1 - Admission & Certs", "Prof. R. Sharma", c1Svcs));

    std::vector<std::string> c2Svcs;
    c2Svcs.push_back("FEE");
    c2Svcs.push_back("GEN");
    counters.push_back(Counter(2, "Counter 2 - Cashier & Fees", "Mrs. S. Deshmukh", c2Svcs));

    std::vector<std::string> c3Svcs;
    c3Svcs.push_back("EXM");
    c3Svcs.push_back("CRT");
    c3Svcs.push_back("GEN");
    counters.push_back(Counter(3, "Counter 3 - Examination Wing", "Mr. A. Patil", c3Svcs));

    std::vector<std::string> c4Svcs;
    c4Svcs.push_back("ADM");
    c4Svcs.push_back("FEE");
    c4Svcs.push_back("EXM");
    c4Svcs.push_back("CRT");
    c4Svcs.push_back("GEN");
    counters.push_back(Counter(4, "Counter 4 - Multi-Service Express", "Mr. V. Khan", c4Svcs));
}

Token QueueEngine::generateToken(const std::string& serviceId, const std::string& studentName,
                                 const std::string& studentId, int priority) {
    std::string sId = serviceId;
    if (services.find(sId) == services.end()) {
        sId = "GEN"; // fallback
    }

    std::string sName = services[sId].getName();
    Token t(nextNumericId++, sId, sName, studentName, studentId, priority);

    queues[sId].enqueue(t);
    saveState();
    return t;
}

Counter* QueueEngine::getCounter(int counterId) {
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getId() == counterId) {
            return &counters[i];
        }
    }
    return nullptr;
}

bool QueueEngine::setCounterStatus(int counterId, CounterStatus newStatus) {
    Counter* c = getCounter(counterId);
    if (!c) return false;
    c->setStatus(newStatus);
    saveState();
    return true;
}

bool QueueEngine::setCounterServices(int counterId, const std::vector<std::string>& serviceIds) {
    Counter* c = getCounter(counterId);
    if (!c) return false;
    c->setSupportedServices(serviceIds);
    saveState();
    return true;
}

// Dynamic Waiting-Time Calculation (Strictly Accurate & 0 min when line is clear)
double QueueEngine::calculateEstimatedWaitMinutes(const std::string& serviceId, int position) const {
    if (position <= 0) return 0.0;

    double avgDuration = 5.0;
    std::map<std::string, ServiceType>::const_iterator sIt = services.find(serviceId);
    if (sIt != services.end()) {
        avgDuration = sIt->second.getCurrentAvgDurationMinutes();
    }

    int availableCompatibleCounters = 0;
    int totalActiveCompatibleCounters = 0;
    double minRemainingBusy = 999.0;
    double totalRemainingBusy = 0.0;

    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getStatus() == COUNTER_OFFLINE || counters[i].getStatus() == COUNTER_PAUSED) continue;

        if (counters[i].canHandleService(serviceId)) {
            totalActiveCompatibleCounters++;
            if (counters[i].getStatus() == COUNTER_AVAILABLE) {
                availableCompatibleCounters++;
            } else if (counters[i].getStatus() == COUNTER_SERVING && counters[i].getHasActiveToken()) {
                double elapsed = counters[i].getElapsedServiceMinutes();
                double rem = avgDuration - elapsed;
                if (rem < 0.5) rem = 0.5;
                if (rem < minRemainingBusy) minRemainingBusy = rem;
                totalRemainingBusy += rem;
            }
        }
    }

    if (totalActiveCompatibleCounters == 0) totalActiveCompatibleCounters = 1;

    // Rule 1: If next in line (position 1) and a counter is free -> 0 minutes wait!
    if (position == 1 && availableCompatibleCounters > 0) {
        return 0.0;
    }

    // Rule 2: If position 1 and all counters are busy -> wait only for earliest busy desk to finish
    if (position == 1 && availableCompatibleCounters == 0) {
        return (minRemainingBusy < 990.0) ? std::ceil(minRemainingBusy) : std::ceil(avgDuration);
    }

    // Rule 3: Students ahead in line
    int peopleAhead = position - 1;
    double wait = (peopleAhead * avgDuration) / static_cast<double>(totalActiveCompatibleCounters);
    if (availableCompatibleCounters == 0 && minRemainingBusy < 990.0) {
        wait += (minRemainingBusy / static_cast<double>(totalActiveCompatibleCounters));
    }

    return (wait <= 0.0) ? 0.0 : std::ceil(wait);
}

// Estimated wait time for a new visitor walking in right now
double QueueEngine::getOverallEstimatedWaitMinutes() const {
    int totalWaiting = getTotalWaitingCount();
    if (totalWaiting == 0) {
        if (getAvailableCounterCount() > 0) {
            return 0.0; // Counter is ready immediately
        }
        // No one waiting, but all counters are serving
        double minRem = 999.0;
        for (size_t i = 0; i < counters.size(); ++i) {
            if (counters[i].getStatus() == COUNTER_SERVING) {
                double elapsed = counters[i].getElapsedServiceMinutes();
                double rem = 5.0 - elapsed;
                if (rem < 0.5) rem = 0.5;
                if (rem < minRem) minRem = rem;
            }
        }
        return (minRem < 990.0) ? std::ceil(minRem) : 0.0;
    }

    // Queue has waiting students: calculate average wait
    int activeCounters = getAvailableCounterCount() + getBusyCounterCount();
    if (activeCounters == 0) activeCounters = 1;

    double avgServiceTime = getOverallAvgServiceDurationMinutes();
    double wait = (totalWaiting * avgServiceTime) / static_cast<double>(activeCounters);
    return std::ceil(wait);
}

// Overall historical average duration of services
double QueueEngine::getOverallAvgServiceDurationMinutes() const {
    if (completedHistory.empty()) return 5.0; // standard baseline
    double totalDuration = 0.0;
    for (size_t i = 0; i < completedHistory.size(); ++i) {
        totalDuration += completedHistory[i].getActualServiceDurationMinutes();
    }
    double avg = totalDuration / static_cast<double>(completedHistory.size());
    return (avg <= 0.0) ? 5.0 : avg;
}

bool QueueEngine::searchToken(int tokenId, Token& outToken, int& outQueuePos,
                             double& outEstimatedWait, std::string& outMsg) const {
    // 1. Check if token is actively being served at any counter
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getHasActiveToken() && counters[i].getCurrentToken().getId() == tokenId) {
            outToken = counters[i].getCurrentToken();
            outQueuePos = 0; // 0 denotes at counter
            outEstimatedWait = 0.0;
            outMsg = "Token is currently being served at " + counters[i].getName() + ".";
            return true;
        }
    }

    // 2. Check each service queue
    for (std::map<std::string, ServiceQueue>::const_iterator it = queues.begin(); it != queues.end(); ++it) {
        int pos = it->second.findPosition(tokenId, outToken);
        if (pos != -1) {
            outQueuePos = pos;
            outEstimatedWait = calculateEstimatedWaitMinutes(it->first, pos);
            std::ostringstream ss;
            ss << "Position #" << pos << " in " << outToken.getServiceName()
               << " queue (" << (pos - 1) << " ahead). Est. wait: ~"
               << static_cast<int>(outEstimatedWait) << " minutes.";
            outMsg = ss.str();
            return true;
        }
    }

    // 3. Check history (Completed, Cancelled, No-Show)
    for (size_t i = 0; i < auditHistory.size(); ++i) {
        if (auditHistory[i].getId() == tokenId) {
            outToken = auditHistory[i];
            outQueuePos = -1;
            outEstimatedWait = 0.0;
            outMsg = "Token has finished service with status: " + outToken.getStatusString();
            return true;
        }
    }

    outMsg = "Token not found.";
    return false;
}

bool QueueEngine::cancelTokenInQueue(int tokenId, const std::string& reason, Token& outCancelled) {
    for (std::map<std::string, ServiceQueue>::iterator it = queues.begin(); it != queues.end(); ++it) {
        if (it->second.removeToken(tokenId, outCancelled)) {
            outCancelled.setStatus(STATUS_CANCELLED);
            outCancelled.setFinishTime(std::time(nullptr));
            outCancelled.setNotes("Cancelled in queue: " + (reason.empty() ? "Cancelled by student" : reason));
            auditHistory.push_back(outCancelled);
            saveState();
            return true;
        }
    }
    return false;
}

bool QueueEngine::markCounterNoShow(int counterId, const std::string& reason, Token& outToken) {
    Counter* c = getCounter(counterId);
    if (!c || !c->getHasActiveToken()) return false;

    if (c->markNoShow(outToken, reason)) {
        auditHistory.push_back(outToken);
        saveState();
        return true;
    }
    return false;
}

bool QueueEngine::markCounterCancelled(int counterId, const std::string& reason, Token& outToken) {
    Counter* c = getCounter(counterId);
    if (!c || !c->getHasActiveToken()) return false;

    if (c->markCancelled(outToken, reason)) {
        auditHistory.push_back(outToken);
        saveState();
        return true;
    }
    return false;
}

bool QueueEngine::isGracePeriodExpired(int counterId) const {
    const Counter* c = nullptr;
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getId() == counterId) {
            c = &counters[i];
            break;
        }
    }
    if (!c || !c->getHasActiveToken()) return false;
    double elapsed = c->getElapsedServiceMinutes();
    return elapsed >= static_cast<double>(gracePeriodMinutes);
}

bool QueueEngine::callNextForCounter(int counterId, Token& outCalledToken) {
    Counter* c = getCounter(counterId);
    if (!c) return false;

    // Counter must not already be serving an active token
    if (c->getHasActiveToken()) return false;
    if (c->getStatus() == COUNTER_OFFLINE || c->getStatus() == COUNTER_PAUSED) return false;

    // Intelligent Selection Strategy across all supported queues:
    // 1. Look for Priority tokens (priority > 0)
    // 2. Look for longest waiting token (earliest arrival timestamp)
    std::string bestServiceId = "";
    Token candidateToken;
    bool foundCandidate = false;

    const std::vector<std::string>& supported = c->getSupportedServices();

    for (size_t i = 0; i < supported.size(); ++i) {
        const std::string& sId = supported[i];
        if (queues.find(sId) == queues.end()) continue;
        if (queues[sId].isEmpty()) continue;

        Token frontToken;
        if (queues[sId].peek(frontToken)) {
            if (!foundCandidate) {
                candidateToken = frontToken;
                bestServiceId = sId;
                foundCandidate = true;
            } else {
                // Priority check
                if (frontToken.getPriority() > candidateToken.getPriority()) {
                    candidateToken = frontToken;
                    bestServiceId = sId;
                } else if (frontToken.getPriority() == candidateToken.getPriority()) {
                    // FIFO earliest arrival
                    if (frontToken.getArrivalTime() < candidateToken.getArrivalTime()) {
                        candidateToken = frontToken;
                        bestServiceId = sId;
                    }
                }
            }
        }
    }

    if (!foundCandidate) return false;

    // Dequeue from the selected service queue in O(1)
    if (queues[bestServiceId].dequeue(outCalledToken)) {
        c->assignToken(outCalledToken);
        outCalledToken = c->getCurrentToken();
        saveState();
        return true;
    }

    return false;
}

bool QueueEngine::manualOverrideCall(int counterId, int tokenId, Token& outCalledToken) {
    Counter* c = getCounter(counterId);
    if (!c) return false;
    if (c->getHasActiveToken()) return false;

    // Search and remove specific token from its queue
    for (std::map<std::string, ServiceQueue>::iterator it = queues.begin(); it != queues.end(); ++it) {
        if (it->second.removeToken(tokenId, outCalledToken)) {
            outCalledToken.setNotes("[Manual Override Assignment]");
            c->assignToken(outCalledToken);
            outCalledToken = c->getCurrentToken();
            saveState();
            return true;
        }
    }

    return false;
}

bool QueueEngine::completeServiceAtCounter(int counterId, Token& outCompletedToken) {
    Counter* c = getCounter(counterId);
    if (!c || !c->getHasActiveToken()) return false;

    if (c->completeCurrent(outCompletedToken)) {
        // Record completed service duration in dynamic average
        double actualMins = outCompletedToken.getActualServiceDurationMinutes();
        if (actualMins > 0.0 && services.find(outCompletedToken.getServiceId()) != services.end()) {
            services[outCompletedToken.getServiceId()].recordCompletedService(actualMins);
        }

        completedHistory.push_back(outCompletedToken);
        auditHistory.push_back(outCompletedToken);
        exportHistoryCsv(dataDirectory + "/completed_records.csv");
        saveState();
        return true;
    }

    return false;
}

double QueueEngine::getCounterProjectedWorkloadMinutes(int counterId) const {
    const Counter* c = nullptr;
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getId() == counterId) {
            c = &counters[i];
            break;
        }
    }
    if (!c || c->getStatus() == COUNTER_OFFLINE || c->getStatus() == COUNTER_PAUSED) return 0.0;

    double workload = 0.0;
    if (c->getHasActiveToken()) {
        double avgDur = 5.0;
        std::map<std::string, ServiceType>::const_iterator sit = services.find(c->getCurrentToken().getServiceId());
        if (sit != services.end()) avgDur = sit->second.getCurrentAvgDurationMinutes();
        double elapsed = c->getElapsedServiceMinutes();
        double remaining = avgDur - elapsed;
        if (remaining > 0) workload += remaining;
    }

    // Add proportional queue workload for supported services
    const std::vector<std::string>& svcs = c->getSupportedServices();
    for (size_t i = 0; i < svcs.size(); ++i) {
        const std::string& sId = svcs[i];
        std::map<std::string, ServiceQueue>::const_iterator qit = queues.find(sId);
        if (qit != queues.end()) {
            int qSize = qit->second.size();
            double dur = 5.0;
            std::map<std::string, ServiceType>::const_iterator sit = services.find(sId);
            if (sit != services.end()) dur = sit->second.getCurrentAvgDurationMinutes();

            // Count how many online counters share this service
            int sharers = 0;
            for (size_t k = 0; k < counters.size(); ++k) {
                if (counters[k].canHandleService(sId) && counters[k].getStatus() != COUNTER_OFFLINE && counters[k].getStatus() != COUNTER_PAUSED) {
                    sharers++;
                }
            }
            if (sharers == 0) sharers = 1;
            workload += (qSize * dur) / static_cast<double>(sharers);
        }
    }

    return std::ceil(workload);
}

void QueueEngine::setQrCodeTargetUrl(const std::string& url) {
    if (!url.empty()) {
        qrCodeTargetUrl = url;
        saveState();
    }
}

std::string QueueEngine::getQrCodeSvg() const {
    return QRCodeGenerator::generateSvg(qrCodeTargetUrl);
}

std::string QueueEngine::getQrCodeDataUri() const {
    return QRCodeGenerator::generateDataUri(qrCodeTargetUrl);
}

int QueueEngine::getTotalWaitingCount() const {
    int total = 0;
    for (std::map<std::string, ServiceQueue>::const_iterator it = queues.begin(); it != queues.end(); ++it) {
        total += it->second.size();
    }
    return total;
}

int QueueEngine::getWaitingCountForService(const std::string& serviceId) const {
    std::map<std::string, ServiceQueue>::const_iterator it = queues.find(serviceId);
    if (it != queues.end()) return it->second.size();
    return 0;
}

int QueueEngine::getAvailableCounterCount() const {
    int count = 0;
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getStatus() == COUNTER_AVAILABLE) count++;
    }
    return count;
}

int QueueEngine::getBusyCounterCount() const {
    int count = 0;
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getStatus() == COUNTER_SERVING) count++;
    }
    return count;
}

int QueueEngine::getPausedCounterCount() const {
    int count = 0;
    for (size_t i = 0; i < counters.size(); ++i) {
        if (counters[i].getStatus() == COUNTER_PAUSED) count++;
    }
    return count;
}

void QueueEngine::updateServiceBaseDuration(const std::string& sId, double mins) {
    if (services.find(sId) != services.end()) {
        services[sId].setBaseAvgDurationMinutes(mins);
        saveState();
    }
}

void QueueEngine::saveState() {
    std::ofstream out((dataDirectory + "/token_config.txt").c_str());
    if (out.is_open()) {
        out << nextNumericId << "\n";
        out << gracePeriodMinutes << "\n";
        out << qrCodeTargetUrl << "\n";
        out.close();
    }

    // Save active queues
    std::ofstream qOut((dataDirectory + "/active_queue.txt").c_str());
    if (qOut.is_open()) {
        for (std::map<std::string, ServiceQueue>::const_iterator it = queues.begin(); it != queues.end(); ++it) {
            std::vector<Token> tokens = it->second.getAllTokens();
            for (size_t i = 0; i < tokens.size(); ++i) {
                const Token& t = tokens[i];
                qOut << "WAITING|" << t.getId() << "|"
                     << t.getServiceId() << "|"
                     << t.getServiceName() << "|"
                     << t.getStudentName() << "|"
                     << t.getStudentId() << "|"
                     << t.getPriority() << "|"
                     << t.getArrivalTime() << "\n";
            }
        }
        qOut.close();
    }
}

void QueueEngine::loadState() {
    std::ifstream in((dataDirectory + "/token_config.txt").c_str());
    if (in.is_open()) {
        int id, grace;
        std::string qrUrl;
        if (in >> id >> grace) {
            if (id >= 101) nextNumericId = id;
            if (grace > 0) gracePeriodMinutes = grace;
            in >> std::ws;
            if (std::getline(in, qrUrl) && !qrUrl.empty()) {
                qrCodeTargetUrl = qrUrl;
            }
        }
        in.close();
    }

    std::ifstream qIn((dataDirectory + "/active_queue.txt").c_str());
    if (qIn.is_open()) {
        std::string line;
        while (std::getline(qIn, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string prefix, idStr, sId, sName, stName, stId, prioStr, arrTimeStr;
            std::getline(ss, prefix, '|');
            std::getline(ss, idStr, '|');
            std::getline(ss, sId, '|');
            std::getline(ss, sName, '|');
            std::getline(ss, stName, '|');
            std::getline(ss, stId, '|');
            std::getline(ss, prioStr, '|');
            std::getline(ss, arrTimeStr, '|');

            int id = std::atoi(idStr.c_str());
            int prio = std::atoi(prioStr.c_str());
            std::time_t arr = static_cast<std::time_t>(std::atoll(arrTimeStr.c_str()));

            Token t(id, sId, sName, stName, stId, prio);
            if (arr > 0) t.setStartTime(arr);
            if (queues.find(sId) != queues.end()) {
                queues[sId].enqueue(t);
            }
        }
        qIn.close();
    }
}

void QueueEngine::exportHistoryCsv(const std::string& filepath) const {
    std::ofstream out(filepath.c_str());
    if (!out.is_open()) return;

    out << "Token ID,Student Name,Student ID,Service Type,Status,Counter,Arrival Time,Call Time,Finish Time,Wait Mins,Service Mins,Notes\n";
    for (size_t i = 0; i < auditHistory.size(); ++i) {
        const Token& t = auditHistory[i];
        out << t.getFormattedId() << ","
            << "\"" << t.getStudentName() << "\","
            << "\"" << t.getStudentId() << "\","
            << "\"" << t.getServiceName() << "\","
            << t.getStatusString() << ","
            << "\"" << t.getCounterName() << "\","
            << t.getArrivalTimeString() << ","
            << t.getCallTimeString() << ","
            << t.getFinishTimeString() << ","
            << t.getActualWaitDurationMinutes() << ","
            << t.getActualServiceDurationMinutes() << ","
            << "\"" << t.getNotes() << "\"\n";
    }
    out.close();
}

void QueueEngine::loadCollegeDemoData() {
    generateToken("ADM", "Aarav Sharma", "SE-2026-012", 0);
    generateToken("FEE", "Priya Patel", "TE-2025-088", 0);
    generateToken("EXM", "Rahul Verma", "BE-2024-045", 1); // priority token
    generateToken("CRT", "Sneha Kulkarni", "ALUMNI-2023", 0);
    generateToken("GEN", "Vikram Singh", "FE-2027-104", 0);
    generateToken("ADM", "Ananya Deshpande", "SE-2026-095", 0);
    generateToken("FEE", "Kunal Bansal", "SE-2026-031", 0);
    generateToken("EXM", "Meera Nair", "TE-2025-119", 0);

    Token called1, called2;
    callNextForCounter(1, called1);
    callNextForCounter(2, called2);

    saveState();
}

void QueueEngine::resetAllQueues() {
    for (std::map<std::string, ServiceQueue>::iterator it = queues.begin(); it != queues.end(); ++it) {
        it->second.clear();
    }
    for (size_t i = 0; i < counters.size(); ++i) {
        Token dummy;
        counters[i].completeCurrent(dummy);
        counters[i].setStatus(COUNTER_AVAILABLE);
    }
    nextNumericId = 101;
    saveState();
}

std::string QueueEngine::getFullStatusJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"totalWaiting\":" << getTotalWaitingCount() << ","
        << "\"availableCounters\":" << getAvailableCounterCount() << ","
        << "\"busyCounters\":" << getBusyCounterCount() << ","
        << "\"pausedCounters\":" << getPausedCounterCount() << ","
        << "\"totalCounters\":" << counters.size() << ","
        << "\"gracePeriodMinutes\":" << gracePeriodMinutes << ","
        << "\"autoWorkloadBalancing\":" << (autoWorkloadBalancing ? "true" : "false") << ","
        << "\"overallAvgServiceDurationMinutes\":" << std::fixed << std::setprecision(1) << getOverallAvgServiceDurationMinutes() << ","
        << "\"overallEstimatedWaitMinutes\":" << std::fixed << std::setprecision(1) << getOverallEstimatedWaitMinutes() << ","
        << "\"qrCodeTargetUrl\":\"" << qrCodeTargetUrl << "\",";

    // Services
    oss << "\"services\":[";
    bool firstSvc = true;
    for (std::map<std::string, ServiceType>::const_iterator it = services.begin(); it != services.end(); ++it) {
        if (!firstSvc) oss << ",";
        oss << it->second.toJson();
        firstSvc = false;
    }
    oss << "],";

    // Counters
    oss << "\"counters\":[";
    for (size_t i = 0; i < counters.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{"
            << "\"id\":" << counters[i].getId() << ","
            << "\"name\":\"" << counters[i].getName() << "\","
            << "\"operatorName\":\"" << counters[i].getOperatorName() << "\","
            << "\"status\":\"" << counters[i].getStatusString() << "\","
            << "\"hasActiveToken\":" << (counters[i].getHasActiveToken() ? "true" : "false") << ","
            << "\"projectedWorkloadMinutes\":" << getCounterProjectedWorkloadMinutes(counters[i].getId()) << ","
            << "\"elapsedMinutes\":" << std::fixed << std::setprecision(1) << counters[i].getElapsedServiceMinutes() << ","
            << "\"isGraceExpired\":" << (isGracePeriodExpired(counters[i].getId()) ? "true" : "false") << ","
            << "\"totalServedCount\":" << counters[i].getTotalServedCount() << ","
            << "\"totalNoShowCount\":" << counters[i].getTotalNoShowCount() << ","
            << "\"totalCancelledCount\":" << counters[i].getTotalCancelledCount() << ","
            << "\"supportedServices\":[";
        const std::vector<std::string>& svcs = counters[i].getSupportedServices();
        for (size_t k = 0; k < svcs.size(); ++k) {
            if (k > 0) oss << ",";
            oss << "\"" << svcs[k] << "\"";
        }
        oss << "],";
        if (counters[i].getHasActiveToken()) {
            oss << "\"currentToken\":" << counters[i].getCurrentToken().toJson();
        } else {
            oss << "\"currentToken\":null";
        }
        oss << "}";
    }
    oss << "],";

    // Queues
    oss << "\"queues\":{";
    bool firstQ = true;
    for (std::map<std::string, ServiceQueue>::const_iterator it = queues.begin(); it != queues.end(); ++it) {
        if (!firstQ) oss << ",";
        oss << "\"" << it->first << "\":" << it->second.toJson();
        firstQ = false;
    }
    oss << "}";

    oss << "}";
    return oss.str();
}

std::string QueueEngine::getHistoryJson() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < auditHistory.size(); ++i) {
        if (i > 0) oss << ",";
        oss << auditHistory[auditHistory.size() - 1 - i].toJson();
    }
    oss << "]";
    return oss.str();
}

std::string QueueEngine::getReportsJson() const {
    std::ostringstream oss;
    int totalCompleted = 0;
    int totalCancelled = 0;
    int totalNoShow = 0;

    for (size_t i = 0; i < auditHistory.size(); ++i) {
        if (auditHistory[i].getStatus() == STATUS_COMPLETED) totalCompleted++;
        else if (auditHistory[i].getStatus() == STATUS_CANCELLED) totalCancelled++;
        else if (auditHistory[i].getStatus() == STATUS_NOSHOW) totalNoShow++;
    }

    oss << "{"
        << "\"totalCompleted\":" << totalCompleted << ","
        << "\"totalCancelled\":" << totalCancelled << ","
        << "\"totalNoShow\":" << totalNoShow << ","
        << "\"totalProcessed\":" << auditHistory.size() << ","
        << "\"overallAvgServiceDurationMinutes\":" << std::fixed << std::setprecision(1) << getOverallAvgServiceDurationMinutes() << ","
        << "\"overallEstimatedWaitMinutes\":" << std::fixed << std::setprecision(1) << getOverallEstimatedWaitMinutes()
        << "}";
    return oss.str();
}
