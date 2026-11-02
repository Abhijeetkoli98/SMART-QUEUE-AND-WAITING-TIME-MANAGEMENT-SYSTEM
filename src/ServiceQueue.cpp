#include "ServiceQueue.h"
#include <sstream>

ServiceQueue::ServiceQueue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

ServiceQueue::~ServiceQueue() {
    clear();
}

ServiceQueue::ServiceQueue(const ServiceQueue& other)
    : frontNode(nullptr), rearNode(nullptr), count(0) {
    QueueNode* curr = other.frontNode;
    while (curr != nullptr) {
        enqueue(curr->data);
        curr = curr->next;
    }
}

ServiceQueue& ServiceQueue::operator=(const ServiceQueue& other) {
    if (this != &other) {
        clear();
        QueueNode* curr = other.frontNode;
        while (curr != nullptr) {
            enqueue(curr->data);
            curr = curr->next;
        }
    }
    return *this;
}

void ServiceQueue::enqueue(const Token& token) {
    QueueNode* newNode = new QueueNode(token);
    if (rearNode == nullptr) {
        frontNode = rearNode = newNode;
    } else {
        rearNode->next = newNode;
        rearNode = newNode;
    }
    count++;
}

bool ServiceQueue::dequeue(Token& outToken) {
    if (isEmpty()) return false;

    QueueNode* temp = frontNode;
    outToken = temp->data;

    frontNode = frontNode->next;
    if (frontNode == nullptr) {
        rearNode = nullptr;
    }

    delete temp;
    count--;
    return true;
}

bool ServiceQueue::peek(Token& outToken) const {
    if (isEmpty()) return false;
    outToken = frontNode->data;
    return true;
}

int ServiceQueue::findPosition(int tokenId, Token& outToken) const {
    QueueNode* curr = frontNode;
    int pos = 1;
    while (curr != nullptr) {
        if (curr->data.getId() == tokenId) {
            outToken = curr->data;
            return pos;
        }
        curr = curr->next;
        pos++;
    }
    return -1;
}

bool ServiceQueue::removeToken(int tokenId, Token& outRemovedToken) {
    if (isEmpty()) return false;

    QueueNode* curr = frontNode;
    QueueNode* prev = nullptr;

    while (curr != nullptr) {
        if (curr->data.getId() == tokenId) {
            outRemovedToken = curr->data;

            if (prev == nullptr) {
                // Removing front node
                frontNode = curr->next;
                if (frontNode == nullptr) rearNode = nullptr;
            } else {
                prev->next = curr->next;
                if (curr == rearNode) rearNode = prev;
            }

            delete curr;
            count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

std::vector<Token> ServiceQueue::getAllTokens() const {
    std::vector<Token> list;
    QueueNode* curr = frontNode;
    while (curr != nullptr) {
        list.push_back(curr->data);
        curr = curr->next;
    }
    return list;
}

void ServiceQueue::clear() {
    while (frontNode != nullptr) {
        QueueNode* temp = frontNode;
        frontNode = frontNode->next;
        delete temp;
    }
    rearNode = nullptr;
    count = 0;
}

std::string ServiceQueue::toJson() const {
    std::ostringstream oss;
    oss << "[";
    QueueNode* curr = frontNode;
    bool first = true;
    while (curr != nullptr) {
        if (!first) oss << ",";
        oss << curr->data.toJson();
        first = false;
        curr = curr->next;
    }
    oss << "]";
    return oss.str();
}

