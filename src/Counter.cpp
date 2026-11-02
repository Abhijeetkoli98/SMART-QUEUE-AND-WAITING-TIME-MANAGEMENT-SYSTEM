#include "Counter.h"
#include <sstream>
#include <iomanip>

static std::string escapeJson(const std::string& s) {
    std::ostringstream o;
    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c == '"') o << "\\\"";
        else if (c == '\\') o << "\\\\";
        else if (c == '\n') o << "\\n";
        else if (c == '\r') o << "\\r";
        else if (c == '\t') o << "\\t";
        else o << c;
    }
    return o.str();
}

Counter::Counter()
    : id(0), name("Counter"), operatorName("Operator"),
      status(COUNTER_OFFLINE), hasActiveToken(false),
      callTimestamp(0), totalServedCount(0), totalNoShowCount(0), totalCancelledCount(0) {}

Counter::Counter(int id, const std::string& name, const std::string& opName,
                 const std::vector<std::string>& services)
    : id(id), name(name), operatorName(opName), supportedServiceIds(services),
      status(COUNTER_AVAILABLE), hasActiveToken(false),
      callTimestamp(0), totalServedCount(0), totalNoShowCount(0), totalCancelledCount(0) {}

std::string Counter::getStatusString() const {
    switch (status) {
        case COUNTER_AVAILABLE: return "AVAILABLE";
        case COUNTER_SERVING:   return "SERVING";
        case COUNTER_PAUSED:    return "PAUSED";
        case COUNTER_OFFLINE:   return "OFFLINE";
        default:                return "UNKNOWN";
    }
}

bool Counter::canHandleService(const std::string& sId) const {
    for (size_t i = 0; i < supportedServiceIds.size(); ++i) {
        if (supportedServiceIds[i] == sId || supportedServiceIds[i] == "ALL") return true;
    }
    return false;
}

void Counter::setStatus(CounterStatus newStatus) {
    // If we transition to OFFLINE or PAUSED while serving, we don't drop the active token
    status = newStatus;
}

void Counter::assignToken(const Token& t) {
    currentToken = t;
    currentToken.setStatus(STATUS_SERVING);
    currentToken.setAssignedCounter(id, name);
    std::time_t now = std::time(nullptr);
    currentToken.setCallTime(now);
    currentToken.setStartTime(now);

    hasActiveToken = true;
    callTimestamp = now;
    status = COUNTER_SERVING;
}

bool Counter::completeCurrent(Token& outToken) {
    if (!hasActiveToken) return false;

    currentToken.setStatus(STATUS_COMPLETED);
    currentToken.setFinishTime(std::time(nullptr));
    outToken = currentToken;

    hasActiveToken = false;
    callTimestamp = 0;
    totalServedCount++;
    status = COUNTER_AVAILABLE;
    return true;
}

bool Counter::markNoShow(Token& outToken, const std::string& reason) {
    if (!hasActiveToken) return false;

    currentToken.setStatus(STATUS_NOSHOW);
    currentToken.setFinishTime(std::time(nullptr));
    currentToken.setNotes("No-Show: " + (reason.empty() ? "Student did not report to counter" : reason));
    outToken = currentToken;

    hasActiveToken = false;
    callTimestamp = 0;
    totalNoShowCount++;
    status = COUNTER_AVAILABLE;
    return true;
}

bool Counter::markCancelled(Token& outToken, const std::string& reason) {
    if (!hasActiveToken) return false;

    currentToken.setStatus(STATUS_CANCELLED);
    currentToken.setFinishTime(std::time(nullptr));
    currentToken.setNotes("Cancelled: " + (reason.empty() ? "Operator cancelled service" : reason));
    outToken = currentToken;

    hasActiveToken = false;
    callTimestamp = 0;
    totalCancelledCount++;
    status = COUNTER_AVAILABLE;
    return true;
}

double Counter::getElapsedServiceMinutes() const {
    if (!hasActiveToken || callTimestamp == 0) return 0.0;
    std::time_t now = std::time(nullptr);
    if (now < callTimestamp) return 0.0;
    return static_cast<double>(now - callTimestamp) / 60.0;
}

std::string Counter::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":" << id << ","
        << "\"name\":\"" << escapeJson(name) << "\","
        << "\"operatorName\":\"" << escapeJson(operatorName) << "\","
        << "\"status\":\"" << getStatusString() << "\","
        << "\"hasActiveToken\":" << (hasActiveToken ? "true" : "false") << ","
        << "\"callTimestamp\":" << callTimestamp << ","
        << "\"elapsedMinutes\":" << std::fixed << std::setprecision(1) << getElapsedServiceMinutes() << ","
        << "\"totalServedCount\":" << totalServedCount << ","
        << "\"totalNoShowCount\":" << totalNoShowCount << ","
        << "\"totalCancelledCount\":" << totalCancelledCount << ","
        << "\"supportedServices\":[";
    for (size_t i = 0; i < supportedServiceIds.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "\"" << escapeJson(supportedServiceIds[i]) << "\"";
    }
    oss << "],";
    if (hasActiveToken) {
        oss << "\"currentToken\":" << currentToken.toJson();
    } else {
        oss << "\"currentToken\":null";
    }
    oss << "}";
    return oss.str();
}

