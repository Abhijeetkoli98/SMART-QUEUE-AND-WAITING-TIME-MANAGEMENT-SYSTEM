# Smart Queue and Waiting-Time Management System
## Master College Project Guide: Full Code Explanation & Viva Questions

> **Academic Year:** Second Year Computer Engineering (SE Computer)  
> **Course:** Data Structures & Object-Oriented Programming (OOP) in C++  
> **Live Web Demo:** [https://smart-queue-and-waiting-time-manage.vercel.app/web_gui/index.html](https://smart-queue-and-waiting-time-manage.vercel.app/web_gui/index.html)  
> **GitHub Repository:** [https://github.com/Abhijeetkoli98/SMART-QUEUE-AND-WAITING-TIME-MANAGEMENT-SYSTEM](https://github.com/Abhijeetkoli98/SMART-QUEUE-AND-WAITING-TIME-MANAGEMENT-SYSTEM)  

---

# TABLE OF CONTENTS
1. [Everything We Used in This Project (Tech Stack & Architecture)](#part-1-everything-we-used-in-this-project)
2. [Detailed Code Explanation (File-by-File & Line-by-Line)](#part-2-detailed-code-explanation)
   - [A. Token Class (`Token.h` & `Token.cpp`)](#a-token-class)
   - [B. Dynamic Linked-List Queue (`Queue.h` & `Queue.cpp`)](#b-dynamic-linked-list-queue)
   - [C. Queue Manager & Persistence (`QueueManager.h` & `QueueManager.cpp`)](#c-queuemanager-class)
   - [D. Interactive Console Interface (`main.cpp`)](#d-console-interface-maincpp)
   - [E. Web Dashboard & Server (`server.js` & `index.html`)](#e-web-dashboard--server)
3. [The Mathematical Waiting-Time Formula](#part-3-mathematical-waiting-time-formula)
4. [What Interviewers Will Ask & What You Should Reply (Viva Q&A)](#part-4-viva-voce-interview-questions--answers)
5. [2-Minute Project Demonstration Speech Script](#part-5-two-minute-demonstration-script)

---

# PART 1: EVERYTHING WE USED IN THIS PROJECT

### 1. Programming Languages & Tools
- **C++ (C++11 Standard):** Used for the core desktop application engine. Provides high execution speed and direct pointer memory management.
- **MinGW GCC Compiler (`g++`):** Compiles the C++ source files into a standalone native Windows executable (`smart_queue.exe`).
- **HTML5, CSS3, & Modern JavaScript:** Used to build the responsive visual web dashboard.
- **Node.js (`http` module):** Powers `server.js` to host the web dashboard locally at `http://localhost:3000` and bridges C++ data files to the web via JSON.
- **Git & GitHub:** Version control and source code repository hosting.
- **Vercel:** Cloud platform used for live deployment.

---

### 2. Data Structure Used: Custom Singly Linked-List Queue
Instead of using a simple array or standard library queue, we built a **Custom Dynamic Linked-List Queue** from scratch using pointers:

```
[frontNode]                                           [rearNode]
     │                                                     │
     ▼                                                     ▼
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ Token: T-101 │ ──> │ Token: T-102 │ ──> │ Token: T-103 │ ──> NULL
└──────────────┘     └──────────────┘     └──────────────┘
 (Served next)                             (Just arrived)
```

#### Why did we choose a Linked List over an Array?
1. **Dynamic Size (No Overflow):** Arrays have a fixed size (e.g. `100`). If 101 customers arrive, the array overflows. A linked list allocates nodes on-demand using `new`.
2. **$O(1)$ Time Complexity for Enqueue & Dequeue:**
   - **Enqueue (Joining line):** Adding a person at `rearNode` takes constant time $O(1)$.
   - **Dequeue (Calling next):** Removing the person at `frontNode` takes constant time $O(1)$.
3. **No Shifting Overhead:** In an array, removing the first item requires shifting all remaining items to the left in $O(n)$ time. In a linked list, we simply advance the pointer: `frontNode = frontNode->next`.

#### Complexity Table
| Operation | Function Name | Linked List Queue | Fixed Array Queue |
|:---|:---|:---:|:---:|
| Add Person | `enqueue()` | **$O(1)$ Constant** | $O(1)$ (Until full) |
| Call Next | `dequeue()` | **$O(1)$ Constant** | $O(n)$ (Shifting required) |
| Peek Front | `peek()` | **$O(1)$ Constant** | $O(1)$ |
| Search Token | `findPosition()` | **$O(n)$ Linear** | $O(n)$ Linear |

---

### 3. Object-Oriented Programming (OOP) Concepts Applied
1. **Encapsulation:** Variables like `tokenId`, `customerName`, and pointers like `frontNode` are `private`. They cannot be tampered with directly and are accessed through controlled public methods.
2. **Classes and Objects:**
   - `Token`: Represents the person/token entity.
   - `Queue`: Manages the dynamic linked nodes and FIFO logic.
   - `QueueManager`: Handles business logic, file I/O, and wait-time estimation.
3. **Constructors and Destructors (RAII):**
   - Constructors initialize default states.
   - The `Queue` destructor (`~Queue()`) traverses all allocated nodes and frees them using `delete`, preventing memory leaks.
4. **Separation of Concerns:** Clean file separation where UI (`main.cpp`), data structure (`Queue`), entity (`Token`), and business operations (`QueueManager`) are independent.

---

### 4. File Storage & Persistence
We use C++ file streams (`<fstream>`):
1. **`data/token_config.txt`:** Saves the next Token ID (e.g., `105`) and the average service time (e.g., `5` mins). When the app restarts, token numbers continue without resetting.
2. **`data/active_queue.txt`:** Saves unserved active tokens. If the system is rebooted, active waiting customers are restored.
3. **`data/completed_records.csv`:** Appends finished visits (Token ID, Name, Service, Arrival, Completion) in standard Comma-Separated Values format that can be opened in Microsoft Excel.

---

# PART 2: DETAILED CODE EXPLANATION

Here is the exact explanation of every file and function so you can explain any line of code to the examiner.

---

## A. Token Class

### File: `src/Token.h`
```cpp
class Token {
private:
    int tokenId;                // Numeric ID, e.g., 101, 102
    std::string customerName;   // Name of the visitor
    std::string serviceType;    // Service (e.g., Doc Verification, Fee Payment)
    std::string arrivalTime;    // Time string when token was created (e.g., 10:24 PM)
    std::string status;         // "WAITING", "SERVING", or "COMPLETED"
```
- **What to tell examiner:**  
  *"The `Token` class models each person waiting for service. It holds their ID, name, requested service, arrival time, and current status. All attributes are private to maintain encapsulation."*

### File: `src/Token.cpp`
```cpp
std::string Token::getFormattedId() const {
    std::ostringstream oss;
    oss << "T-" << std::setw(3) << std::setfill('0') << tokenId;
    return oss.str();
}
```
- **What this does:** Converts integer `101` into a professional string `"T-101"`. If the number is single-digit like `5`, `std::setw(3)` and `std::setfill('0')` formats it as `"T-005"`.

---

## B. Dynamic Linked-List Queue

### File: `src/Queue.h`
```cpp
struct Node {
    Token data;     // The Token object stored in this node
    Node* next;     // Pointer to the next node in line

    Node(const Token& token) : data(token), next(nullptr) {}
};
```
- **What this does:** Each customer is wrapped inside a `Node`. `next` points to the customer behind them in line. When a node is created, `next` defaults to `nullptr`.

```cpp
class Queue {
private:
    Node* frontNode; // Points to the customer at the head (served next)
    Node* rearNode;  // Points to the most recently arrived customer (at the back)
    int count;       // Total number of waiting people
```
- **What to tell examiner:**  
  *"Our Queue maintains two pointers: `frontNode` for removing customers when their turn comes, and `rearNode` for adding new customers. `count` gives us the queue length in $O(1)$ time without traversing."*

### File: `src/Queue.cpp`

#### 1. Enqueue Operation (Adding to Queue)
```cpp
void Queue::enqueue(const Token& token) {
    Node* newNode = new Node(token);
    if (rearNode == nullptr) {
        // Queue was empty: both front and rear point to the new node
        frontNode = rearNode = newNode;
    } else {
        // Link the current rear node to the new node, then advance rear
        rearNode->next = newNode;
        rearNode = newNode;
    }
    count++;
}
```
- **Line-by-line explanation:**
  1. `new Node(token)`: Allocates dynamic heap memory for the new visitor.
  2. If `rearNode == nullptr`, the queue was empty, so both `frontNode` and `rearNode` point to this first node.
  3. Otherwise, `rearNode->next = newNode` links the new person behind the current last person, and `rearNode = newNode` updates the rear pointer.
  4. Takes **$O(1)$ constant time** because no loops are involved.

#### 2. Dequeue Operation (Calling Next Person)
```cpp
bool Queue::dequeue(Token& outToken) {
    if (isEmpty()) return false;

    Node* temp = frontNode;        // 1. Save pointer to front node
    outToken = temp->data;         // 2. Copy the token data to output

    frontNode = frontNode->next;   // 3. Advance front to the next person
    if (frontNode == nullptr) {    // 4. If queue is now empty, reset rear
        rearNode = nullptr;
    }

    delete temp;                   // 5. Free memory of removed node
    count--;
    return true;
}
```
- **Line-by-line explanation:**
  1. Checks if the queue is empty. If empty, returns `false` safely.
  2. Points temporary pointer `temp` to `frontNode`.
  3. Advances `frontNode = frontNode->next`.
  4. Calls `delete temp` to free the allocated heap memory so there is **no memory leak**.
  5. Takes **$O(1)$ constant time**.

#### 3. Searching Position
```cpp
int Queue::findPosition(int tokenId, Token& outFoundToken) const {
    Node* current = frontNode;
    int position = 1;

    while (current != nullptr) {
        if (current->data.getId() == tokenId) {
            outFoundToken = current->data;
            return position; // 1-based position in queue
        }
        current = current->next;
        position++;
    }
    return -1; // Not found
}
```
- **Explanation:** Traverses from `frontNode` to the end. Position `1` means the person is next in line. Takes $O(n)$ time in the worst case.

#### 4. Destructor (Memory Cleanup)
```cpp
Queue::~Queue() {
    clear();
}

void Queue::clear() {
    while (frontNode != nullptr) {
        Node* temp = frontNode;
        frontNode = frontNode->next;
        delete temp; // Free every node
    }
    rearNode = nullptr;
    count = 0;
}
```
- **What to tell examiner:**  
  *"When the `Queue` object goes out of scope, the destructor automatically loops through any remaining nodes and calls `delete` to release heap memory back to the operating system."*

---

## C. QueueManager Class

### File: `src/QueueManager.h` & `src/QueueManager.cpp`
`QueueManager` coordinates the entire application.

#### Key Functions:
1. `generateToken(name, service)`:
   - Gets the current system timestamp (`07:30 PM`).
   - Creates a new `Token` with `nextTokenId`.
   - Calls `waitingQueue.enqueue(newToken)`.
   - Increments `nextTokenId++`.
   - Calls `saveStateToFile()` and `saveActiveQueueToFile()`.

2. `callNextPerson(outToken)`:
   - Checks if the queue is empty.
   - Dequeues the front person into `currentlyServing`.
   - Sets `isServing = true`.
   - Updates `active_queue.txt`.

3. `completeCurrentService(outCompletedToken)`:
   - Sets status to `"COMPLETED"`.
   - Opens `data/completed_records.csv` in append mode (`std::ios::app`).
   - Writes: `Token ID, Name, Service, Arrival Time, Completion Time, Status`.
   - Sets `isServing = false`.

4. `calculateWaitTime(position)`:
   ```cpp
   int peopleAhead = position - 1;
   int wait = peopleAhead * avgServiceTimeMinutes;
   if (isServing) {
       wait += avgServiceTimeMinutes;
   }
   return wait;
   ```

---

## D. Console Interface (`main.cpp`)

- **Input Sanitization:** Uses `getIntInput()` with `std::cin.fail()` checks:
  ```cpp
  if (std::cin >> value) {
      return value;
  } else {
      std::cin.clear(); // Clear error state
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid characters
  }
  ```
  If a user accidentally types `"abc"`, the program will **never crash or enter an infinite loop**.

---

## E. Web Dashboard & Server

- **`server.js`:** Written in Node.js. Serves static files and provides the `/api/data` endpoint, which reads `data/token_config.txt`, `data/active_queue.txt`, and `data/completed_records.csv` and returns them as a JSON object.
- **`web_gui/index.html`:** Modern dashboard that provides:
  - Responsive metric cards (Currently Serving, Next Token, Total Waiting, Avg Service Time).
  - Speech synthesis (`window.speechSynthesis`) that reads out called token numbers aloud.
  - Interactive modals for project information and search.

---

# PART 3: MATHEMATICAL WAITING-TIME FORMULA

### The Formula:
$$\text{Estimated Wait Time (mins)} = (N_{\text{ahead}} \times T_{\text{avg}}) + (C_{\text{busy}} \times T_{\text{avg}})$$

Where:
- $P$ = Token's 1-based position in queue ($1$ = next in line).
- $N_{\text{ahead}} = P - 1$ = Number of people waiting ahead.
- $T_{\text{avg}}$ = Average service time per customer (configurable, default = 5 mins).
- $C_{\text{busy}}$ = 1 if someone is currently at the counter, or 0 if the counter is idle.

### Practical Example to Give the Examiner:
> *"Suppose Rahul has token `T-104` and is at **Position #4** in line.*  
> *Number of people ahead of him = $4 - 1 = 3$ people.*  
> *If each service takes an average of **5 minutes**, and someone is currently at the counter:*  
> $$\text{Wait Time} = (3 \times 5) + (1 \times 5) = 20 \text{ minutes}$$  
> *The system will tell Rahul: **'Queue Position: #4 | Est. Wait: ~20 minutes'**."*

---

# PART 4: VIVA VOCE INTERVIEW QUESTIONS & ANSWERS

Here are the questions examiners typically ask second-year students, along with simple answers:

### Q1: What is the main objective of your project?
**Your Reply:**
> *"The objective of our project is to eliminate physical lines at places like college offices, clinics, and banks. Instead of standing without knowing when their turn will come, visitors receive a unique token number, are placed in a First-In, First-Out queue, and can see their exact position and estimated waiting time."*

---

### Q2: Why did you choose C++ for this project?
**Your Reply:**
> *"We chose C++ because it gives us direct control over memory using pointers, executes at high speed, and supports Object-Oriented Programming (OOP) concepts like classes, objects, and encapsulation."*

---

### Q3: What is FIFO and why is it essential here?
**Your Reply:**
> *"FIFO stands for **First-In, First-Out**. It means the person who joins the queue first is served first. In a queue management system, FIFO guarantees fairness and prevents line-cutting."*

---

### Q4: Why did you build a Linked-List Queue instead of using an Array?
**Your Reply:**
> *"In an array, size is fixed at compile time, leading to queue overflow or wasted memory. Also, removing the front element in an array requires shifting all remaining elements in $O(n)$ time. With our Linked List Queue:  
> 1. Nodes are allocated dynamically on-demand using `new`.  
> 2. Both `enqueue` (at rear) and `dequeue` (at front) take strictly **$O(1)$ constant time**."*

---

### Q5: What is the difference between `frontNode` and `rearNode`?
**Your Reply:**
> *" `frontNode` points to the customer at the head of the queue who will be served next. `rearNode` points to the customer at the back who just arrived. New tokens are added at `rearNode`, and called tokens are removed from `frontNode`."*

---

### Q6: What happens to the dynamically allocated memory when someone leaves the queue?
**Your Reply:**
> *"In our `dequeue()` function, we assign a temporary pointer to the front node, advance `frontNode` to the next node, and call `delete temp`. This frees the allocated heap memory and prevents memory leaks."*

---

### Q7: How do you prevent token numbers from restarting from 101 when the software restarts?
**Your Reply:**
> *"We persist the state in `data/token_config.txt`. Every time a token is issued, the counter is saved. When the application starts up, `QueueManager` reads this file. If the last token was 104, the next token generated will be 105."*

---

### Q8: What happens if the computer crashes or is accidentally closed? Is the waiting line lost?
**Your Reply:**
> *"No, sir/ma'am. We implemented state recovery in `data/active_queue.txt`. Every time someone is enqueued or called, the active queue is serialized to disk. Upon startup, `loadActiveQueueFromFile()` restores the exact queue order."*

---

### Q9: How is the estimated waiting time calculated?
**Your Reply:**
> *"We use the formula:  
> `Wait Time = (Queue Position - 1) * Average Service Time`.  
> If an operator is actively serving someone at the counter, we add 1 service interval to account for the ongoing transaction. The average service duration can also be adjusted by the operator in the menu."*

---

### Q10: How did you implement Encapsulation in your project?
**Your Reply:**
> *"In `Token` and `Queue`, all internal data attributes—like `tokenId`, `customerName`, `frontNode`, and `rearNode`—are marked `private`. External classes can only access or modify them through public member functions like `getId()`, `enqueue()`, and `dequeue()`."*

---

### Q11: What will happen if someone enters invalid text when a number is expected?
**Your Reply:**
> *"Our input helper function checks `std::cin.fail()`. If invalid input like letters is entered, it clears the error flag with `std::cin.clear()` and flushes the input buffer with `std::cin.ignore()`, then prompts the user again without crashing."*

---

### Q12: Why did you also build a Web Dashboard?
**Your Reply:**
> *"The C++ console application is the core backend engine. We created the Web Dashboard to show how modern display monitors in waiting halls can visualize the queue, update counters, and use speech synthesis to announce called tokens aloud to visitors."*

---

### Q13: What are the limitations of this project and future scope?
**Your Reply:**
> *"Currently, the system models a single service counter with a fixed average service duration. For future scope, we can expand it to support multiple parallel counters (Counter 1, 2, 3), add SMS notifications via Twilio, and use a SQL database for multi-branch sync."*

---

# PART 5: TWO-MINUTE DEMONSTRATION SCRIPT

Use this script during your college demonstration:

> *"Good morning respected examiners.*
>
> *Today, I am demonstrating our project: the **Smart Queue and Waiting-Time Management System** developed in C++.*
>
> *In busy service centers like banks, hospitals, and college offices, physical queues cause confusion, exhaustion, and disputes. Our software digitizes this process.*
>
> *Here is our application running. At the top of the screen, our dashboard displays the currently serving customer, the next token in line, the total waiting count, and the average service duration.*
>
> *(Demonstrating Option 8)*  
> *I will select Option 8 to load sample visitors. You can see tokens T-101 through T-104 are generated.*
>
> *(Demonstrating Option 2)*  
> *Selecting Option 2 displays our waiting queue in strict First-In, First-Out (FIFO) order using our custom Singly Linked List Queue.*
>
> *(Demonstrating Option 5)*  
> *If visitor T-103 wants to check their wait time, I select Option 5 and enter 103. The system shows they are at Position #3, with 2 people ahead, resulting in an estimated wait of 10 minutes.*
>
> *(Demonstrating Option 3 & 4)*  
> *When the operator clicks 'Call Next', Token T-101 moves to the service counter in O(1) time. Once served, selecting Option 4 completes the service and saves the transaction record to `completed_records.csv`.*
>
> *(Demonstrating Web Dashboard)*  
> *We have also deployed a live web version on Vercel at `smart-queue-and-waiting-time-manage.vercel.app`, which includes voice announcements for waiting halls.*
>
> *Thank you. I am ready for questions."*
