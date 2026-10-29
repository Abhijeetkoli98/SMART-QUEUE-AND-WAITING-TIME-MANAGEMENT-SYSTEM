# College Project Report
## Smart Queue and Waiting-Time Management System Using C++

**Course:** Second Year Computer Engineering (SE Computer)  
**Academic Subject:** Data Structures & Object-Oriented Programming in C++  
**Academic Year:** 2026  
**Language:** C++ (Standard C++11 / GCC MinGW)  

---

### 1. Abstract
Long physical queues at service counters such as college administration offices, banking halls, clinics, and government service centers often cause severe overcrowding, customer frustration, lack of transparency, and inefficient staff utilization. This project introduces the **Smart Queue and Waiting-Time Management System**, an automated, object-oriented desktop software developed in C++. The system dispenses unique sequential tokens to customers upon arrival, organizes them into a dynamic **First-In, First-Out (FIFO) Linked-List Queue**, estimates waiting times in real-time based on configurable average service duration, and enables operators to call, serve, and log completed visits into local persistent records (`CSV` and text configuration files). The solution eliminates physical queuing, provides clear waiting estimates, prevents data loss across system restarts, and optimizes counter workflows.

---

### 2. Introduction & Problem Statement

#### 2.1 Problem Statement
In traditional manual queue systems:
1. **Uncertainty & Anxiety:** People have no estimation of how long they will wait.
2. **Disorder & Line Cutting:** Physical standing queues lead to arguments and confusion regarding arrival order.
3. **Loss of Productivity:** Visitors cannot sit down, step aside, or attend to other tasks while waiting.
4. **Lack of Audit Records:** Service centers lack historical logs of who was served, at what time, and how long transactions took.

#### 2.2 Proposed Solution
The **Smart Queue Management System** replaces the physical line with an automated digital queue:
- When a person arrives, the operator enters their name and service type.
- A unique token number (e.g., `T-101`, `T-102`) is assigned.
- The person is placed in an ordered FIFO queue.
- At any time, a customer can query their token number to see:
  - Exact position in line (e.g., `#3 in line`).
  - How many people are ahead of them (`2 people ahead`).
  - Estimated waiting time in minutes (`10 minutes`).
- Operators call the next person with a single command, which updates the live counter display.
- Once finished, the record is automatically written to `completed_records.csv`.
- If the application is closed or restarted, token numbers and active queues are preserved.

---

### 3. Objectives of the Project
1. **FIFO Queue Discipline:** Ensure absolute fairness by serving customers strictly in arrival sequence.
2. **Dynamic Data Structure Implementation:** Use a custom linked-list queue to demonstrate dynamic memory management, pointers, and $O(1)$ enqueue/dequeue operations.
3. **Transparent Waiting-Time Estimation:** Compute and display dynamic wait times using the formula:
   $$\text{Estimated Wait Time} = (\text{People Ahead}) \times (\text{Average Service Time}) + (\text{Active Counter Time if Busy})$$
4. **Data Persistence:** Store token counters, active queue states, and completed service logs in local files without requiring heavy database dependencies.
5. **Beginner-Friendly, Modular Design:** Structure code cleanly into modular classes (`Token`, `Queue`, `QueueManager`, `main`).

---

### 4. Hardware & Software Requirements

#### Hardware Requirements:
- **Processor:** Intel Core i3 / AMD Ryzen 3 or higher
- **RAM:** 2 GB minimum (4 GB recommended)
- **Disk Space:** 50 MB free space
- **Display:** 1024x768 resolution minimum

#### Software Requirements:
- **Operating System:** Windows 7 / 8 / 10 / 11, Linux, or macOS
- **Compiler:** MinGW GCC C++ Compiler (`g++` supporting C++11 or higher)
- **Editor / IDE:** Visual Studio Code, Code::Blocks, Dev-C++, or Notepad++
- **Shell:** Windows Command Prompt (CMD) or PowerShell

---

### 5. System Architecture & Component Interaction

```
       +-------------------------------------------------------+
       |               Operator / User Interface               |
       |  (Console Menu: main.cpp / Web Dashboard: index.html) |
       +-------------------------------------------------------+
                                   |
                                   v
       +-------------------------------------------------------+
       |                     QueueManager                      |
       |  - Token Generator (Unique IDs: T-101, T-102...)      |
       |  - Wait-Time Estimator: People Ahead * AvgTime        |
       |  - Counter Dispatcher (Currently Serving)             |
       +-------------------------------------------------------+
                     |                             |
                     v                             v
       +---------------------------+   +-----------------------+
       | Custom Linked-List Queue  |   | File Storage Manager  |
       | (FIFO: Front -> Rear)     |   | - token_config.txt    |
       | Node: Token + Node* next  |   | - active_queue.txt    |
       | Enqueue: O(1) at Rear     |   | - completed_records   |
       | Dequeue: O(1) at Front    |   +-----------------------+
       +---------------------------+
```

#### Why Choose a Custom Linked-List Queue over an Array?
1. **Dynamic Capacity:** Arrays have a fixed capacity (e.g. `arr[100]`). If 101 people arrive, an array overflows. A linked list allocates nodes on-demand using `new`.
2. **True $O(1)$ Operations:** With `frontNode` and `rearNode` pointers, enqueueing at the rear and dequeueing at the front are strictly $O(1)$ constant time operations.
3. **No Memory Shifting:** In a simple array queue, removing an element from index 0 requires shifting all remaining elements left, taking $O(n)$ time. The linked list simply moves `frontNode = frontNode->next`, taking $O(1)$.

---

### 6. Mathematical Waiting-Time Model

Let:
- $P$ = 1-based position of the queried token in the waiting queue ($P = 1$ is next in line).
- $N_{\text{ahead}} = P - 1$ = Number of people waiting ahead in the queue.
- $T_{\text{avg}}$ = Configured average service duration per person (default = 5 minutes).
- $C_{\text{busy}}$ = 1 if a person is currently being served at the counter, or 0 if the counter is idle.

$$\text{Estimated Wait Time (mins)} = (N_{\text{ahead}} \times T_{\text{avg}}) + (C_{\text{busy}} \times T_{\text{avg}})$$

**Example:**
- Rohan holds Token `T-103`.
- His queue position is `#3`.
- People ahead in line = $3 - 1 = 2$.
- The counter is currently serving Token `T-101` ($C_{\text{busy}} = 1$).
- Average service time = 5 minutes.
$$\text{Wait Time} = (2 \times 5) + (1 \times 5) = 15 \text{ minutes}$$

> **Note on Approximations:** This calculation assumes average service time. Actual service times can fluctuate depending on transaction complexity (e.g., quick fee payment vs. lengthy document dispute).

---

### 7. Class Structure & OOP Principles Applied

1. **Encapsulation:**
   - Class `Token`: All private member variables (`tokenId`, `customerName`, `serviceType`, `arrivalTime`, `status`) are accessed only through getter and setter methods.
   - Class `Queue`: Internal node pointers `frontNode` and `rearNode` are completely hidden from client code.
2. **Single Responsibility Principle (SRP):**
   - `Token`: Models customer entity and token state.
   - `Queue`: Manages linked-list pointer logic and FIFO operations.
   - `QueueManager`: Handles business logic, file persistence, wait time estimation, and counter transitions.
   - `main.cpp`: Manages console input/output and presentation loop.
3. **Resource Management (RAII):**
   - The `Queue` destructor iterates through any remaining nodes and calls `delete` to prevent memory leaks.

---

### 8. Algorithms

#### 8.1 Enqueue Algorithm (Adding to Queue)
```
Algorithm ENQUEUE(token):
1. Allocate newNode using memory allocation (new Node(token))
2. Set newNode->next = NULL
3. IF rearNode == NULL THEN:
      frontNode = rearNode = newNode
   ELSE:
      rearNode->next = newNode
      rearNode = newNode
   END IF
4. Increment count by 1
```

#### 8.2 Dequeue Algorithm (Calling Next)
```
Algorithm DEQUEUE(outToken):
1. IF frontNode == NULL THEN:
      Return FALSE (Queue is empty)
   END IF
2. Set temp = frontNode
3. outToken = temp->data
4. frontNode = frontNode->next
5. IF frontNode == NULL THEN:
      rearNode = NULL
   END IF
6. Free memory for temp (delete temp)
7. Decrement count by 1
8. Return TRUE
```

#### 8.3 Token Search & Position Finding
```
Algorithm FIND_POSITION(searchId):
1. Set current = frontNode
2. Set position = 1
3. WHILE current != NULL DO:
      IF current->data.getId() == searchId THEN:
         Return position
      END IF
      current = current->next
      position = position + 1
   END WHILE
4. Return -1 (Token not found)
```

---

### 9. File Storage & Persistence Mechanism

1. **`data/token_config.txt`**:
   - Stores the next sequential token ID (e.g. `105`) and the average service time (e.g. `5`).
   - Ensures token numbering does not restart at 101 when the software restarts.
2. **`data/active_queue.txt`**:
   - Saves unserved tokens and the currently serving person.
   - Read during startup to seamlessly restore queue state.
3. **`data/completed_records.csv`**:
   - Standard Comma-Separated Values log file.
   - Can be opened in Microsoft Excel or Google Sheets for administrative reporting.

---

### 10. Limitations & Honest Assessment
1. **Single Service Counter:** The core C++ version demonstrates one service counter. Multiple parallel counters (e.g., Counter 1, Counter 2, Counter 3) can be added as future scope.
2. **Fixed Average Service Time:** Service time is estimated as an average rather than calculated from live machine learning predictions.
3. **Local Machine Storage:** Uses local text/CSV files rather than a centralized cloud database, making it ideal for standalone office kiosks.

---

### 11. Conclusion
The **Smart Queue and Waiting-Time Management System** successfully addresses the challenge of chaotic physical queues. By applying fundamental computer engineering principles—specifically Object-Oriented Programming, dynamic linked lists, FIFO queuing discipline, and file handling—the system provides a reliable, transparent, and user-friendly experience for both counter operators and waiting customers.

