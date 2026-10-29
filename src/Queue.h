#ifndef QUEUE_H
#define QUEUE_H

#include "Token.h"

// Node structure for the linked list queue
struct Node {
    Token data;
    Node* next;

    Node(const Token& token) : data(token), next(nullptr) {}
};

// Custom Linked List Queue implementing strict FIFO
class Queue {
private:
    Node* frontNode; // Points to the person next in line
    Node* rearNode;  // Points to the most recently added person
    int count;       // Total number of people currently waiting

public:
    Queue();
    ~Queue();

    // Core Queue Operations
    void enqueue(const Token& token); // Add person to back of queue
    bool dequeue(Token& outToken);     // Remove person from front of queue
    bool peek(Token& outToken) const;  // Look at next person without removing
    bool isEmpty() const;
    int size() const;

    // Search and Position calculation
    // Returns 1-based position in queue (1 = next to be served), or -1 if not found
    int findPosition(int tokenId, Token& outFoundToken) const;

    // Display all tokens currently in queue
    void displayQueue() const;

    // Getter for front node (useful for serialization / saving)
    const Node* getFront() const { return frontNode; }

    // Clear entire queue and free memory
    void clear();
};

#endif // QUEUE_H

