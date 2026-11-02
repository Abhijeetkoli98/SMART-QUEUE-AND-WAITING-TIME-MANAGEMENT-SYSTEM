#ifndef QUEUE_ENGINE_H
#define QUEUE_ENGINE_H

#include "Token.h"
#include "ServiceType.h"
#include "ServiceQueue.h"
#include "Counter.h"
#include <map>
#include <vector>
#include <string>

class QueueEngine {
private:
    std::map<std::string, ServiceType> services;
    std::map<std::string, ServiceQueue> queues;
    std::vector<Counter> counters;

    std::vector<Token> completedHistory;
    std::vector<Token> auditHistory; // All completed, cancelled, no-show

    int nextNumericId;
    int gracePeriodMinutes; // e.g. 5 minutes before no-show can be declared
    bool autoWorkloadBalancing; // whether available counters automatically pull tokens

    std::string dataDirectory;

public:
    QueueEngine(const std::string& dataDir = "data");
    ~QueueEngine();

    // Initialization & Default Setup
    void initializeDefaultCollegeServices();
    void initializeDefaultCounters();

    // Feature A: Token Generation & Service Queues
    Token generateToken(const std::string& serviceId, const std::string& studentName,
                        const std::string& studentId, int priority = 0);

    // Feature B: Counter Management
    bool setCounterStatus(int counterId, CounterStatus newStatus);
    bool setCounterServices(int counterId, const std::vector<std::string>& serviceIds);
    Counter* getCounter(int counterId);
    const std::vector<Counter>& getCounters() const { return counters; }

    // Feature C: Dynamic Waiting-Time Calculation
    // Calculates estimated wait in minutes for a given token position & service
    double calculateEstimatedWaitMinutes(const std::string& serviceId, int position) const;
    bool searchToken(int tokenId, Token& outToken, int& outQueuePos,
                     double& outEstimatedWait, std::string& outMsg) const;

    // Feature D: No-Show and Cancellation Management
    bool cancelTokenInQueue(int tokenId, const std::string& reason, Token& outCancelled);
    bool markCounterNoShow(int counterId, const std::string& reason, Token& outToken);
    bool markCounterCancelled(int counterId, const std::string& reason, Token& outToken);
    int getGracePeriodMinutes() const { return gracePeriodMinutes; }
    void setGracePeriodMinutes(int m) { if (m > 0) gracePeriodMinutes = m; }
    bool isGracePeriodExpired(int counterId) const;

    // Feature E: Intelligent Workload Balancing & Dispatch
    // Automatically picks the best eligible token for this counter based on FIFO & priority
    bool callNextForCounter(int counterId, Token& outCalledToken);
    // Manual operator override: call a specific token ID
    bool manualOverrideCall(int counterId, int tokenId, Token& outCalledToken);
    // Mark current service completed at counter, update service dynamic average
    bool completeServiceAtCounter(int counterId, Token& outCompletedToken);
    // Workload metric for a counter in estimated minutes
    double getCounterProjectedWorkloadMinutes(int counterId) const;

    // Reporting & Aggregate Statistics
    int getTotalWaitingCount() const;
    int getWaitingCountForService(const std::string& serviceId) const;
    int getAvailableCounterCount() const;
    double getOverallAvgWaitMinutes() const;

    // Settings & State
    void setAutoWorkloadBalancing(bool enabled) { autoWorkloadBalancing = enabled; }
    bool getAutoWorkloadBalancing() const { return autoWorkloadBalancing; }
    void updateServiceBaseDuration(const std::string& sId, double mins);

    // Persistence
    void saveState();
    void loadState();
    void exportHistoryCsv(const std::string& filepath) const;

    // Demo Data
    void loadCollegeDemoData();
    void resetAllQueues();

    // Complete JSON payload for web dashboard
    std::string getFullStatusJson() const;
    std::string getHistoryJson() const;
    std::string getReportsJson() const;
};

#endif // QUEUE_ENGINE_H

