# System Architecture & Design Diagrams
## Smart Queue and Waiting-Time Management System (C++)

This document contains the visual diagrams and design specifications for reports and project presentations.

---

### 1. UML Class Diagram

```mermaid
classDiagram
    class Token {
        -int tokenId
        -string customerName
        -string serviceType
        -string arrivalTime
        -string status
        +Token()
        +Token(id, name, service, time)
        +getId() int
        +getName() string
        +getService() string
        +getArrivalTime() string
        +getStatus() string
        +getFormattedId() string
        +setStatus(newStatus) void
        +displaySummary() void
    }

    class Node {
        +Token data
        +Node* next
        +Node(token)
    }

    class Queue {
        -Node* frontNode
        -Node* rearNode
        -int count
        +Queue()
        +~Queue()
        +enqueue(token) void
        +dequeue(outToken) bool
        +peek(outToken) bool
        +isEmpty() bool
        +size() int
        +findPosition(id, outToken) int
        +displayQueue() void
        +clear() void
    }

    class QueueManager {
        -Queue waitingQueue
        -Token currentlyServing
        -bool isServing
        -int nextTokenId
        -int avgServiceTimeMinutes
        -string counterFilePath
        -string recordsFilePath
        -string activeQueueFilePath
        +QueueManager(dataDir)
        +~QueueManager()
        +generateToken(name, service) Token
        +callNextPerson(outToken) bool
        +completeCurrentService(outToken) bool
        +searchToken(id, outPos, outToken, outWait) bool
        +calculateWaitTime(position) int
        +displayWaitingQueue() void
        +displayCurrentStatus() void
        +displayCompletedRecords() void
        +setAvgServiceTime(minutes) void
    }

    Node "1" *-- "1" Token : contains
    Queue "1" *-- "many" Node : links
    QueueManager "1" o-- "1" Queue : manages
    QueueManager "1" o-- "1" Token : serving
```

---

### 2. UML Use-Case Diagram

```mermaid
flowchart LR
    Operator((Operator / Staff))
    Customer((Customer / Visitor))

    subgraph Smart Queue System
        UC1[Generate New Token]
        UC2[View Live Waiting Queue]
        UC3[Call Next Customer]
        UC4[Mark Service Completed]
        UC5[Search Token & Estimate Wait]
        UC6[View Completed History]
        UC7[Configure Service Time]
        UC8[Save State to File]
    end

    Customer --> UC1
    Customer --> UC5
    Operator --> UC1
    Operator --> UC2
    Operator --> UC3
    Operator --> UC4
    Operator --> UC5
    Operator --> UC6
    Operator --> UC7

    UC1 -.-> UC8
    UC3 -.-> UC8
    UC4 -.-> UC8
```

---

### 3. Program Execution Flowchart

```mermaid
flowchart TD
    Start([Application Start]) --> LoadFiles[Load token_config.txt & active_queue.txt]
    LoadFiles --> MenuDisplay[Display System Dashboard & Menu Options]
    MenuDisplay --> ReadChoice{User Choice}

    ReadChoice -->|1| GenToken[Enter Name & Service -> Enqueue -> Print Token]
    ReadChoice -->|2| ViewQ[Traverse Linked List -> Display Table]
    ReadChoice -->|3| CallNext{Queue Empty?}
    CallNext -->|Yes| WarnEmpty[Show Empty Queue Notice]
    CallNext -->|No| Deq[Dequeue Front Node -> Set Currently Serving]
    ReadChoice -->|4| Complete{Is Serving?}
    Complete -->|Yes| LogCSV[Append to completed_records.csv -> Reset Counter]
    Complete -->|No| WarnIdle[Counter is already idle]
    ReadChoice -->|5| Search[Enter Token ID -> Find Position -> Calculate Wait Time]
    ReadChoice -->|6| ShowHist[Read completed_records.csv]
    ReadChoice -->|7| SetTime[Update Avg Service Time]
    ReadChoice -->|8| Demo[Load 4 Sample Tokens]
    ReadChoice -->|0| ExitProg[Save State to disk -> Exit]

    GenToken --> MenuDisplay
    ViewQ --> MenuDisplay
    WarnEmpty --> MenuDisplay
    Deq --> MenuDisplay
    LogCSV --> MenuDisplay
    WarnIdle --> MenuDisplay
    Search --> MenuDisplay
    ShowHist --> MenuDisplay
    SetTime --> MenuDisplay
    Demo --> MenuDisplay
    ExitProg --> EndNode([Program Terminated])
```

