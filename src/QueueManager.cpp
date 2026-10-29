#include "QueueManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

// Helper to get formatted current system time
std::string QueueManager::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%I:%M %p", localTime);
    return std::string(buffer);
}

// Constructor
QueueManager::QueueManager(const std::string& dataDir)
    : isServing(false),
      nextTokenId(101),
      avgServiceTimeMinutes(5),
      counterFilePath(dataDir + "/token_config.txt"),
      recordsFilePath(dataDir + "/completed_records.csv"),
      activeQueueFilePath(dataDir + "/active_queue.txt")
{
    loadStateFromFile();
    loadActiveQueueFromFile();
}

// Destructor
QueueManager::~QueueManager() {
    saveStateToFile();
    saveActiveQueueToFile();
}

// Save nextTokenId and avgServiceTime
void QueueManager::saveStateToFile() {
    std::ofstream outFile(counterFilePath.c_str());
    if (outFile.is_open()) {
        outFile << nextTokenId << "\n";
        outFile << avgServiceTimeMinutes << "\n";
        outFile.close();
    }
}

// Load nextTokenId and avgServiceTime
void QueueManager::loadStateFromFile() {
    std::ifstream inFile(counterFilePath.c_str());
    if (inFile.is_open()) {
        int id, avgTime;
        if (inFile >> id >> avgTime) {
            if (id >= 101) nextTokenId = id;
            if (avgTime > 0) avgServiceTimeMinutes = avgTime;
        }
        inFile.close();
    }
}

// Save active waiting queue to file
void QueueManager::saveActiveQueueToFile() {
    std::ofstream outFile(activeQueueFilePath.c_str());
    if (!outFile.is_open()) return;

    // First line: is someone currently being served?
    if (isServing) {
        outFile << "SERVING|" << currentlyServing.getId() << "|" 
                << currentlyServing.getName() << "|" 
                << currentlyServing.getService() << "|" 
                << currentlyServing.getArrivalTime() << "\n";
    } else {
        outFile << "NONE\n";
    }

    // Save pending tokens
    const Node* current = waitingQueue.getFront();
    while (current != nullptr) {
        const Token& t = current->data;
        outFile << "WAITING|" << t.getId() << "|" 
                << t.getName() << "|" 
                << t.getService() << "|" 
                << t.getArrivalTime() << "\n";
        current = current->next;
    }
    outFile.close();
}

// Load active waiting queue from file
void QueueManager::loadActiveQueueFromFile() {
    std::ifstream inFile(activeQueueFilePath.c_str());
    if (!inFile.is_open()) return;

    std::string line;
    // Check first line for currently serving
    if (std::getline(inFile, line)) {
        if (line.rfind("SERVING|", 0) == 0) {
            std::stringstream ss(line);
            std::string prefix, idStr, name, service, arrTime;
            std::getline(ss, prefix, '|');
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, service, '|');
            std::getline(ss, arrTime, '|');

            int id = std::atoi(idStr.c_str());
            currentlyServing = Token(id, name, service, arrTime);
            currentlyServing.setStatus("SERVING");
            isServing = true;
        }
    }

    // Load remaining waiting tokens
    while (std::getline(inFile, line)) {
        if (line.rfind("WAITING|", 0) == 0) {
            std::stringstream ss(line);
            std::string prefix, idStr, name, service, arrTime;
            std::getline(ss, prefix, '|');
            std::getline(ss, idStr, '|');
            std::getline(ss, name, '|');
            std::getline(ss, service, '|');
            std::getline(ss, arrTime, '|');

            int id = std::atoi(idStr.c_str());
            Token t(id, name, service, arrTime);
            waitingQueue.enqueue(t);
        }
    }
    inFile.close();
}

// Generate a new token and enqueue
Token QueueManager::generateToken(const std::string& name, const std::string& service) {
    std::string arrival = getCurrentTimestamp();
    Token newToken(nextTokenId, name, service, arrival);
    waitingQueue.enqueue(newToken);

    nextTokenId++;
    saveStateToFile();
    saveActiveQueueToFile();

    return newToken;
}

// Call next person in line
bool QueueManager::callNextPerson(Token& outToken) {
    if (waitingQueue.isEmpty()) {
        return false;
    }

    waitingQueue.dequeue(outToken);
    outToken.setStatus("SERVING");
    currentlyServing = outToken;
    isServing = true;

    saveActiveQueueToFile();
    return true;
}

// Mark current person as completed and record in CSV
bool QueueManager::completeCurrentService(Token& outCompletedToken) {
    if (!isServing) {
        return false;
    }

    currentlyServing.setStatus("COMPLETED");
    outCompletedToken = currentlyServing;
    isServing = false;

    // Append to CSV log
    std::ifstream checkFile(recordsFilePath.c_str());
    bool fileExists = checkFile.good();
    checkFile.close();

    std::ofstream recordFile(recordsFilePath.c_str(), std::ios::app);
    if (recordFile.is_open()) {
        if (!fileExists) {
            // Write CSV header
            recordFile << "Token ID,Customer Name,Service Type,Arrival Time,Completion Time,Status\n";
        }
        recordFile << outCompletedToken.getFormattedId() << ","
                   << "\"" << outCompletedToken.getName() << "\","
                   << "\"" << outCompletedToken.getService() << "\","
                   << outCompletedToken.getArrivalTime() << ","
                   << getCurrentTimestamp() << ","
                   << "COMPLETED\n";
        recordFile.close();
    }

    saveActiveQueueToFile();
    return true;
}

// Search for token by numeric ID
bool QueueManager::searchToken(int tokenId, int& outPosition, Token& outToken, int& outEstimatedWait) const {
    // First check if it is currently being served
    if (isServing && currentlyServing.getId() == tokenId) {
        outPosition = 0; // 0 indicates currently at the counter
        outToken = currentlyServing;
        outEstimatedWait = 0;
        return true;
    }

    int pos = waitingQueue.findPosition(tokenId, outToken);
    if (pos != -1) {
        outPosition = pos;
        outEstimatedWait = calculateWaitTime(pos);
        return true;
    }

    return false;
}

// Estimated wait time in minutes
// If position == 1, 0 people ahead in queue => 0 mins waiting ahead (or avgTime if someone is at counter)
int QueueManager::calculateWaitTime(int position) const {
    if (position <= 0) return 0;

    int peopleAhead = position - 1;
    int wait = peopleAhead * avgServiceTimeMinutes;

    // If an agent is actively serving someone, add the current person's remaining estimate
    if (isServing) {
        wait += avgServiceTimeMinutes;
    }

    return wait;
}

// Display waiting queue
void QueueManager::displayWaitingQueue() const {
    waitingQueue.displayQueue();
}

// Display top summary cards
void QueueManager::displayCurrentStatus() const {
    std::cout << "\n  +----------------------- SYSTEM DASHBOARD -----------------------+\n";
    std::cout << "  | Currently Serving : " << std::left << std::setw(42)
              << (isServing ? (currentlyServing.getFormattedId() + " (" + currentlyServing.getName() + ")") : "No one (Counter Available)")
              << " |\n";

    Token nextToken;
    std::string nextStr = waitingQueue.peek(nextToken) ? (nextToken.getFormattedId() + " (" + nextToken.getName() + ")") : "None (Queue Empty)";
    std::cout << "  | Next In Line      : " << std::left << std::setw(42) << nextStr << " |\n";
    std::cout << "  | Total Waiting     : " << std::left << std::setw(42) << (std::to_string(waitingQueue.size()) + " person(s)") << " |\n";
    std::cout << "  | Avg Service Time  : " << std::left << std::setw(42) << (std::to_string(avgServiceTimeMinutes) + " minutes per person") << " |\n";
    std::cout << "  +----------------------------------------------------------------+\n";
}

// Display completed records from file
void QueueManager::displayCompletedRecords() const {
    std::ifstream recordFile(recordsFilePath.c_str());
    if (!recordFile.is_open()) {
        std::cout << "  [!] No completed service records found yet.\n";
        return;
    }

    std::string line;
    bool isFirst = true;
    std::cout << "\n  ==================== COMPLETED SERVICE HISTORY ====================\n";
    while (std::getline(recordFile, line)) {
        if (line.empty()) continue;
        if (isFirst) {
            std::cout << "  " << line << "\n";
            std::cout << "  " << std::string(75, '-') << "\n";
            isFirst = false;
        } else {
            std::cout << "  " << line << "\n";
        }
    }
    std::cout << "  ===================================================================\n";
    recordFile.close();
}

void QueueManager::setAvgServiceTime(int minutes) {
    if (minutes > 0) {
        avgServiceTimeMinutes = minutes;
        saveStateToFile();
    }
}

int QueueManager::getAvgServiceTime() const {
    return avgServiceTimeMinutes;
}

int QueueManager::getTotalWaiting() const {
    return waitingQueue.size();
}

bool QueueManager::hasCurrentlyServing() const {
    return isServing;
}

Token QueueManager::getCurrentlyServing() const {
    return currentlyServing;
}

int QueueManager::getNextTokenId() const {
    return nextTokenId;
}

