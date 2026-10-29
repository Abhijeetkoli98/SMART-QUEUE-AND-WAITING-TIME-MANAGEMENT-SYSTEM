#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include "Queue.h"
#include <string>

class QueueManager {
private:
    Queue waitingQueue;
    Token currentlyServing;
    bool isServing;
    int nextTokenId;
    int avgServiceTimeMinutes; // Average time spent per person (e.g. 5 mins)

    // File paths
    std::string counterFilePath;
    std::string recordsFilePath;
    std::string activeQueueFilePath;

    // Helper functions
    std::string getCurrentTimestamp() const;
    void saveStateToFile();
    void loadStateFromFile();
    void saveActiveQueueToFile();
    void loadActiveQueueFromFile();

public:
    QueueManager(const std::string& dataDir = "data");
    ~QueueManager();

    // Core Business Operations
    Token generateToken(const std::string& name, const std::string& service);
    bool callNextPerson(Token& outToken);
    bool completeCurrentService(Token& outCompletedToken);
    
    // Search & Estimation
    bool searchToken(int tokenId, int& outPosition, Token& outToken, int& outEstimatedWait) const;
    int calculateWaitTime(int position) const;

    // Display & Reports
    void displayWaitingQueue() const;
    void displayCurrentStatus() const;
    void displayCompletedRecords() const;

    // Settings & State
    void setAvgServiceTime(int minutes);
    int getAvgServiceTime() const;
    int getTotalWaiting() const;
    bool hasCurrentlyServing() const;
    Token getCurrentlyServing() const;
    int getNextTokenId() const;
};

#endif // QUEUE_MANAGER_H

