#ifndef COUNTER_H
#define COUNTER_H

#include "Token.h"
#include <string>
#include <vector>

enum CounterStatus {
    COUNTER_AVAILABLE,
    COUNTER_SERVING,
    COUNTER_PAUSED,
    COUNTER_OFFLINE
};

class Counter {
private:
    int id;
    std::string name;
    std::string operatorName;
    std::vector<std::string> supportedServiceIds; // e.g. {"ADM", "CRT"}
    CounterStatus status;

    Token currentToken;
    bool hasActiveToken;
    std::time_t callTimestamp;

    int totalServedCount;
    int totalNoShowCount;
    int totalCancelledCount;

public:
    Counter();
    Counter(int id, const std::string& name, const std::string& opName,
            const std::vector<std::string>& services);

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getOperatorName() const { return operatorName; }
    const std::vector<std::string>& getSupportedServices() const { return supportedServiceIds; }
    CounterStatus getStatus() const { return status; }
    std::string getStatusString() const;
    bool getHasActiveToken() const { return hasActiveToken; }
    Token getCurrentToken() const { return currentToken; }
    std::time_t getCallTimestamp() const { return callTimestamp; }
    int getTotalServedCount() const { return totalServedCount; }
    int getTotalNoShowCount() const { return totalNoShowCount; }
    int getTotalCancelledCount() const { return totalCancelledCount; }

    // Service compatibility
    bool canHandleService(const std::string& sId) const;

    // Operations
    void setStatus(CounterStatus newStatus);
    void setOperatorName(const std::string& op) { operatorName = op; }
    void setSupportedServices(const std::vector<std::string>& svcs) { supportedServiceIds = svcs; }

    void assignToken(const Token& t);
    bool completeCurrent(Token& outToken);
    bool markNoShow(Token& outToken, const std::string& reason);
    bool markCancelled(Token& outToken, const std::string& reason);

    // Workload calculation
    double getElapsedServiceMinutes() const;

    std::string toJson() const;
};

#endif // COUNTER_H

