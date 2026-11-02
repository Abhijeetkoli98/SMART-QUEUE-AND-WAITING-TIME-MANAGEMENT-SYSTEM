#ifndef SERVICE_QUEUE_H
#define SERVICE_QUEUE_H

#include "Token.h"
#include <vector>

// Linked List Node for FIFO Queue
struct QueueNode {
    Token data;
    QueueNode* next;

    QueueNode(const Token& t) : data(t), next(nullptr) {}
};

class ServiceQueue {
private:
    QueueNode* frontNode; // Head of FIFO queue
    QueueNode* rearNode;  // Tail of FIFO queue
    int count;            // Number of waiting tokens

public:
    ServiceQueue();
    ~ServiceQueue();

    // Copying a linked list requires deep copy
    ServiceQueue(const ServiceQueue& other);
    ServiceQueue& operator=(const ServiceQueue& other);

    // Core FIFO Queue Operations (O(1))
    void enqueue(const Token& token);
    bool dequeue(Token& outToken);
    bool peek(Token& outToken) const;
    bool isEmpty() const { return frontNode == nullptr; }
    int size() const { return count; }

    // Search and Position (1-based position in queue)
    int findPosition(int tokenId, Token& outToken) const;

    // Remove specific token from queue (e.g. for cancellation or manual override)
    bool removeToken(int tokenId, Token& outRemovedToken);

    // Retrieve all waiting tokens in FIFO order
    std::vector<Token> getAllTokens() const;

    // Clear and free all allocated nodes
    void clear();

    // JSON serialization
    std::string toJson() const;
};

#endif // SERVICE_QUEUE_H

