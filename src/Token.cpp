#include "Token.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// Default constructor
Token::Token() : tokenId(0), customerName(""), serviceType("General"), arrivalTime(""), status("WAITING") {}

// Parameterized constructor
Token::Token(int id, const std::string& name, const std::string& service, const std::string& time)
    : tokenId(id), customerName(name), serviceType(service), arrivalTime(time), status("WAITING") {}

int Token::getId() const {
    return tokenId;
}

std::string Token::getName() const {
    return customerName;
}

std::string Token::getService() const {
    return serviceType;
}

std::string Token::getArrivalTime() const {
    return arrivalTime;
}

std::string Token::getStatus() const {
    return status;
}

std::string Token::getFormattedId() const {
    std::ostringstream oss;
    oss << "T-" << std::setw(3) << std::setfill('0') << tokenId;
    return oss.str();
}

void Token::setStatus(const std::string& newStatus) {
    status = newStatus;
}

void Token::displaySummary() const {
    std::cout << "Token: " << getFormattedId()
              << " | Name: " << customerName
              << " | Service: " << serviceType
              << " | Arrived: " << arrivalTime
              << " | Status: " << status << std::endl;
}

