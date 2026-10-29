#include "Queue.h"
#include <iostream>
#include <iomanip>

Queue::Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

Queue::~Queue() {
    clear();
}

void Queue::enqueue(const Token& token) {
    Node* newNode = new Node(token);
    if (rearNode == nullptr) {
        // Queue is currently empty
        frontNode = rearNode = newNode;
    } else {
        // Queue has existing elements: link new node to rear and advance rear
        rearNode->next = newNode;
        rearNode = newNode;
    }
    count++;
}

bool Queue::dequeue(Token& outToken) {
    if (isEmpty()) {
        return false;
    }

    Node* temp = frontNode;
    outToken = temp->data;

    frontNode = frontNode->next;
    if (frontNode == nullptr) {
        // If the queue became empty after removal, reset rear as well
        rearNode = nullptr;
    }

    delete temp;
    count--;
    return true;
}

bool Queue::peek(Token& outToken) const {
    if (isEmpty()) {
        return false;
    }
    outToken = frontNode->data;
    return true;
}

bool Queue::isEmpty() const {
    return frontNode == nullptr;
}

int Queue::size() const {
    return count;
}

int Queue::findPosition(int tokenId, Token& outFoundToken) const {
    Node* current = frontNode;
    int position = 1; // 1 means next in line

    while (current != nullptr) {
        if (current->data.getId() == tokenId) {
            outFoundToken = current->data;
            return position;
        }
        current = current->next;
        position++;
    }

    return -1; // Not found
}

void Queue::displayQueue() const {
    if (isEmpty()) {
        std::cout << "  [!] The waiting queue is currently empty.\n";
        return;
    }

    std::cout << "  "
              << std::left << std::setw(6)  << "Pos"
              << std::setw(10) << "Token ID"
              << std::setw(20) << "Customer Name"
              << std::setw(18) << "Service"
              << std::setw(14) << "Arrival"
              << "Status" << std::endl;
    std::cout << "  " << std::string(75, '-') << std::endl;

    Node* current = frontNode;
    int pos = 1;
    while (current != nullptr) {
        std::cout << "  #"
                  << std::left << std::setw(5)  << pos
                  << std::setw(10) << current->data.getFormattedId()
                  << std::setw(20) << current->data.getName()
                  << std::setw(18) << current->data.getService()
                  << std::setw(14) << current->data.getArrivalTime()
                  << current->data.getStatus() << std::endl;
        current = current->next;
        pos++;
    }
}

void Queue::clear() {
    while (frontNode != nullptr) {
        Node* temp = frontNode;
        frontNode = frontNode->next;
        delete temp;
    }
    rearNode = nullptr;
    count = 0;
}

