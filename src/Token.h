#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ctime>

// Status of a token through its lifecycle
enum TokenStatus {
    STATUS_WAITING,
    STATUS_CALLED,
    STATUS_SERVING,
    STATUS_COMPLETED,
    STATUS_CANCELLED,
    STATUS_NOSHOW
};

class Token {
private:
    int id;                      // Unique sequential numeric ID (e.g. 101)
    std::string serviceId;       // "ADM", "FEE", "EXM", "CRT"
    std::string serviceName;     // "Admission & Enrollment", etc.
    std::string studentName;     // Visitor / Student Name
    std::string studentId;       // e.g. "SE-2026-042" or phone/roll
    int priority;                // 0 = Normal, 1 = Priority / Specially-abled / Faculty
    TokenStatus status;
    int assignedCounterId;       // Counter ID serving this token (0 = unassigned)
    std::string counterName;     // e.g. "Counter 1 - Admission"
    
    std::time_t arrivalTime;     // When token was created
    std::time_t callTime;        // When operator called the token
    std::time_t startTime;       // When service began
    std::time_t finishTime;      // When service completed / cancelled / noshow
    std::string notes;           // Remarks, cancellation reason, or override note

public:
    Token();
    Token(int id, const std::string& sId, const std::string& sName,
          const std::string& name, const std::string& stId, int prio = 0);

    // Getters
    int getId() const { return id; }
    std::string getServiceId() const { return serviceId; }
    std::string getServiceName() const { return serviceName; }
    std::string getStudentName() const { return studentName; }
    std::string getStudentId() const { return studentId; }
    int getPriority() const { return priority; }
    TokenStatus getStatus() const { return status; }
    int getAssignedCounterId() const { return assignedCounterId; }
    std::string getCounterName() const { return counterName; }
    std::time_t getArrivalTime() const { return arrivalTime; }
    std::time_t getCallTime() const { return callTime; }
    std::time_t getStartTime() const { return startTime; }
    std::time_t getFinishTime() const { return finishTime; }
    std::string getNotes() const { return notes; }

    // Setters
    void setStatus(TokenStatus s) { status = s; }
    void setAssignedCounter(int counterId, const std::string& cName);
    void setCallTime(std::time_t t) { callTime = t; }
    void setStartTime(std::time_t t) { startTime = t; }
    void setFinishTime(std::time_t t) { finishTime = t; }
    void setNotes(const std::string& n) { notes = n; }

    // Helpers
    std::string getFormattedId() const; // e.g. "ADM-101"
    std::string getStatusString() const;
    std::string getArrivalTimeString() const;
    std::string getCallTimeString() const;
    std::string getFinishTimeString() const;
    double getActualServiceDurationMinutes() const;
    double getActualWaitDurationMinutes() const;

    // JSON serialization for HTTP API
    std::string toJson() const;
};

#endif // TOKEN_H
