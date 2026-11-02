#include "Token.h"
#include <sstream>
#include <iomanip>

static std::string formatTimeStr(std::time_t t) {
    if (t == 0) return "--:--";
    std::tm* lt = std::localtime(&t);
    if (!lt) return "--:--";
    char buf[32];
    std::strftime(buf, sizeof(buf), "%I:%M %p", lt);
    return std::string(buf);
}

static std::string escapeJson(const std::string& s) {
    std::ostringstream o;
    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c == '"') o << "\\\"";
        else if (c == '\\') o << "\\\\";
        else if (c == '\b') o << "\\b";
        else if (c == '\f') o << "\\f";
        else if (c == '\n') o << "\\n";
        else if (c == '\r') o << "\\r";
        else if (c == '\t') o << "\\t";
        else o << c;
    }
    return o.str();
}

Token::Token()
    : id(0), serviceId("GEN"), serviceName("General"),
      studentName(""), studentId(""), priority(0),
      status(STATUS_WAITING), assignedCounterId(0), counterName(""),
      arrivalTime(0), callTime(0), startTime(0), finishTime(0), notes("") {}

Token::Token(int i, const std::string& sId, const std::string& sName,
             const std::string& name, const std::string& stId, int prio)
    : id(i), serviceId(sId), serviceName(sName),
      studentName(name), studentId(stId), priority(prio),
      status(STATUS_WAITING), assignedCounterId(0), counterName(""),
      arrivalTime(std::time(nullptr)), callTime(0), startTime(0), finishTime(0), notes("") {}

void Token::setAssignedCounter(int counterId, const std::string& cName) {
    assignedCounterId = counterId;
    counterName = cName;
}

std::string Token::getFormattedId() const {
    std::ostringstream oss;
    oss << serviceId << "-" << std::setw(3) << std::setfill('0') << id;
    return oss.str();
}

std::string Token::getStatusString() const {
    switch (status) {
        case STATUS_WAITING:   return "WAITING";
        case STATUS_CALLED:    return "CALLED";
        case STATUS_SERVING:   return "SERVING";
        case STATUS_COMPLETED: return "COMPLETED";
        case STATUS_CANCELLED: return "CANCELLED";
        case STATUS_NOSHOW:    return "NOSHOW";
        default:               return "UNKNOWN";
    }
}

std::string Token::getArrivalTimeString() const {
    return formatTimeStr(arrivalTime);
}

std::string Token::getCallTimeString() const {
    return formatTimeStr(callTime);
}

std::string Token::getFinishTimeString() const {
    return formatTimeStr(finishTime);
}

double Token::getActualServiceDurationMinutes() const {
    if (startTime == 0 || finishTime == 0 || finishTime < startTime) return 0.0;
    return static_cast<double>(finishTime - startTime) / 60.0;
}

double Token::getActualWaitDurationMinutes() const {
    if (arrivalTime == 0) return 0.0;
    std::time_t end = (callTime != 0) ? callTime : ((finishTime != 0) ? finishTime : std::time(nullptr));
    if (end < arrivalTime) return 0.0;
    return static_cast<double>(end - arrivalTime) / 60.0;
}

std::string Token::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":" << id << ","
        << "\"formattedId\":\"" << escapeJson(getFormattedId()) << "\","
        << "\"serviceId\":\"" << escapeJson(serviceId) << "\","
        << "\"serviceName\":\"" << escapeJson(serviceName) << "\","
        << "\"studentName\":\"" << escapeJson(studentName) << "\","
        << "\"studentId\":\"" << escapeJson(studentId) << "\","
        << "\"priority\":" << priority << ","
        << "\"status\":\"" << getStatusString() << "\","
        << "\"assignedCounterId\":" << assignedCounterId << ","
        << "\"counterName\":\"" << escapeJson(counterName) << "\","
        << "\"arrivalTime\":" << arrivalTime << ","
        << "\"arrivalTimeString\":\"" << getArrivalTimeString() << "\","
        << "\"callTime\":" << callTime << ","
        << "\"callTimeString\":\"" << getCallTimeString() << "\","
        << "\"startTime\":" << startTime << ","
        << "\"finishTime\":" << finishTime << ","
        << "\"finishTimeString\":\"" << getFinishTimeString() << "\","
        << "\"waitDurationMinutes\":" << std::fixed << std::setprecision(1) << getActualWaitDurationMinutes() << ","
        << "\"serviceDurationMinutes\":" << std::fixed << std::setprecision(1) << getActualServiceDurationMinutes() << ","
        << "\"notes\":\"" << escapeJson(notes) << "\""
        << "}";
    return oss.str();
}
