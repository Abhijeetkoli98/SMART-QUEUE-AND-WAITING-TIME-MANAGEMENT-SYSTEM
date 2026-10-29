#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// Token class represents a single person waiting in line
class Token {
private:
    int tokenId;                // Unique ID number, e.g., 101, 102
    std::string customerName;   // Name of the customer/student/patient
    std::string serviceType;    // Service requested (e.g., Cashier, Account, Inquiry)
    std::string arrivalTime;    // Timestamp when token was generated
    std::string status;         // "WAITING", "SERVING", "COMPLETED"

public:
    // Default constructor
    Token();

    // Parameterized constructor
    Token(int id, const std::string& name, const std::string& service, const std::string& time);

    // Getters
    int getId() const;
    std::string getName() const;
    std::string getService() const;
    std::string getArrivalTime() const;
    std::string getStatus() const;

    // Formatted token string, e.g. "T-101"
    std::string getFormattedId() const;

    // Setters
    void setStatus(const std::string& newStatus);

    // Display token information in a clean single line or block
    void displaySummary() const;
};

#endif // TOKEN_H

