# Smart Queue & Waiting-Time Management System (Genuine C++ Core)
## Complete Project Guide, Code Walkthrough & Examiner Viva Voce Preparation

> **Course:** Second Year Computer Engineering (SE Computer)  
> **Topic:** Data Structures & Object-Oriented Programming (OOP) in C++  
> **Key Architecture:** 100% Genuine C++ Core Backend with Native Winsock HTTP/JSON REST API & Clean Web Client  
> **Live Web Demo:** [https://smart-queue-and-waiting-time-manage.vercel.app/web_gui/index.html](https://smart-queue-and-waiting-time-manage.vercel.app/web_gui/index.html)  
> **GitHub Repository:** [https://github.com/Abhijeetkoli98/SMART-QUEUE-AND-WAITING-TIME-MANAGEMENT-SYSTEM](https://github.com/Abhijeetkoli98/SMART-QUEUE-AND-WAITING-TIME-MANAGEMENT-SYSTEM)  

---

# TABLE OF CONTENTS
1. [Executive Summary & Core Principle: Strictly C++](#1-executive-summary--core-principle-strictly-c)
2. [Detailed Breakdown of the 5 Core Features](#2-detailed-breakdown-of-the-5-core-features)
   - [Feature A: Multiple Service Queues](#feature-a-multiple-service-queues)
   - [Feature B: Multiple Counters Management](#feature-b-multiple-counters-management)
   - [Feature C: Dynamic Waiting-Time Estimation](#feature-c-dynamic-waiting-time-estimation)
   - [Feature D: No-Show & Cancellation Management](#feature-d-no-show--cancellation-management)
   - [Feature E: Intelligent Workload Balancing](#feature-e-intelligent-workload-balancing)
3. [Deep-Dive Code Walkthrough (File-by-File & Line-by-Line)](#3-deep-dive-code-walkthrough)
   - [`Token.h` & `Token.cpp`](#a-token-class)
   - [`ServiceType.h`](#b-servicetype-class)
   - [`ServiceQueue.h` & `ServiceQueue.cpp` (Dynamic Linked List FIFO Queue)](#c-servicequeue-class)
   - [`Counter.h` & `Counter.cpp`](#d-counter-class)
   - [`QueueEngine.h` & `QueueEngine.cpp` (Central Brain)](#e-queueengine-class)
   - [`HttpServer.h` & `HttpServer.cpp` (Native Winsock HTTP Server)](#f-httpserver-class)
   - [`main.cpp` (Console CLI & Server Host)](#g-maincpp)
   - [`web_gui/index.html` (Thin Web UI Client)](#h-web-gui)
4. [Time and Space Complexity Analysis](#4-time-and-space-complexity-analysis)
5. [What Interviewers Will Ask & What You Should Reply (Viva Q&A)](#5-what-interviewers-will-ask--what-you-should-reply)
6. [2-Minute Live Demonstration Speaking Script](#6-2-minute-live-demonstration-speaking-script)

---

# 1. EXECUTIVE SUMMARY & CORE PRINCIPLE: STRICTLY C++

This college project implements a complete, enterprise-grade **Smart Queue & Waiting-Time Management System**.

### ⚠️ Strict Architectural Rule:
- **100% of the core logic, state management, queue operations, workload balancing algorithms, QR code generation, calculations, and data persistence are implemented in C++.**
- The frontend (HTML5/CSS3/JavaScript) contains **zero business logic**. It functions strictly as a presentation layer that sends asynchronous HTTP JSON requests (`fetch`) to the native C++ backend running on port `3000`.
- The C++ backend hosts a genuine native HTTP REST server built with Windows Winsock2 (`ws2_32`). It directly parses HTTP requests and returns JSON without any third-party framework or Node.js runtime requirement.

---

# 2. DETAILED BREAKDOWN OF THE 6 MAIN SECTIONS

### Section 1: Dashboard (Admin Overview)
- **Top Metrics Cards:** Total Waiting Students, Available Counters, Busy Counters, Paused/Offline Counters, Average Service Duration (EMA), and Estimated Waiting Time (strictly 0 mins when queues are empty).
- **Active Service Counters:** Real-time counter cards showing operator name, current token, elapsed service time, and current workload.
- **Native C++ QR Code Card:** Real-time SVG rendering of the mobile Student Kiosk URL (`http://172.17.134.16:3000/web_gui/index.html?tab=studentKiosk`), allowing any smartphone on campus Wi-Fi to scan and join the queue. Also features a direct SVG download button.
- **Live Service Queues Summary:** Quick overview of the 5 services: `ADM`, `FEE`, `EXM`, `CRT`, and `GEN`.

### Section 2: Student Kiosk (Token Generation & Verification)
- **Token Generation Form:** Student selects from 5 college services (Admission, Fees, Examination, Certificates, General Enquiry), enters Name and Roll Number, selects Priority status if applicable, and clicks "Generate Token".
- **Double-Submit Protection:** Prevents duplicate token creation.
- **Token Result Card:** Displays unique token number (e.g. `ADM-101`), student name, queue position, and dynamic estimated wait time calculated in C++.
- **Token Search & Status Check:** Students can enter their numeric token ID to check live position and remaining wait time.

### Section 3: Operator Panel (Counter Management)
- **Assigned Counters Station:** Displays Counter 1 to 4 with operator names and compatible services.
- **Operator Actions:** "Call Next Student (Intelligent Workload Balancing)", "Manual Override: Call Specific Token", "Mark Service Completed", "Mark No-Show", "Cancel Service", and counter status toggles (Available / Pause / Offline).
- **Grace Period Indicator:** Automatically highlights when student arrival grace period (5 mins) has expired.

### Section 4: Public Display Monitor (Student Information Screen)
- **Large Waiting Hall Screen:** Designed for TV displays in waiting lobbies with high-contrast dark navy theme (`#0f172a`).
- **NOW SERVING Screen:** Prominently highlights currently serving token number, counter number, student name, and service type.
- **Live Campus Clock & Audio Chime:** Text-to-speech / audio announcement when tokens are called.

### Section 5: Queue & History (FIFO Queues & Audit Log)
- **Live FIFO Queues View:** Displays dynamic singly linked-list queues for each service, showing exact FIFO positions (`#1`, `#2`...), Token IDs, student roll numbers, arrival timestamps, and priority tags.
- **Complete Token History View:** Search and filter historical records by status (Completed, Cancelled, No-Show) and service type.

### Section 6: Settings & Reports (Analytics, CSV Export, QR Config)
- **Performance Analytics Cards:** Total Processed, Completed Services, Cancelled Tokens, No-Show Incidents, Average Service Duration.
- **CSV Export:** Direct download of `completed_records.csv` generated by C++.
- **System Configuration:** Dynamic updates to No-Show Grace Period and Mobile Kiosk QR Code Target URL persisted into C++ engine.
- **Demo Controls:** Quick buttons to load realistic sample demo data or reset queues.

---

### Campus Services & Counters Configuration:
1. **`ADM` — Admission & Registration:** Base ~8 mins &bull; Handled by Counter 1, 4
2. **`FEE` — Fees & Accounts:** Base ~4 mins &bull; Handled by Counter 2, 4
3. **`EXM` — Examination & Results:** Base ~6 mins &bull; Handled by Counter 3, 4
4. **`CRT` — Transcripts & Certificates:** Base ~5 mins &bull; Handled by Counter 1, 3, 4
5. **`GEN` — General Enquiry:** Base ~3 mins &bull; Handled by Counter 1, 2, 3, 4

- **Counter 1 (Prof. R. Sharma):** Handles `ADM`, `CRT`, `GEN`.
- **Counter 2 (Mrs. S. Deshmukh):** Handles `FEE`, `GEN`.
- **Counter 3 (Mr. A. Patil):** Handles `EXM`, `CRT`, `GEN`.
- **Counter 4 (Mr. V. Khan):** Multi-Service Express Counter handling all 5 services.

Each counter has an explicit lifecycle state:
- `AVAILABLE`: Ready to accept the next student.
- `SERVING`: Currently serving a student (displays token ID, student name, elapsed minutes).
- `PAUSED`: Operator on temporary break; will not be assigned new tokens.
- `OFFLINE`: Counter closed.

---

### Feature C: Dynamic Waiting-Time Estimation
Unlike basic systems that show a static or fake number, our C++ engine calculates waiting time using actual service history and multi-counter throughput:

$$\text{Estimated Wait Time} = \max\left(1, \left\lceil \frac{(P - 1) \times T_{\text{avg}}(S)}{N_{\text{active}}(S)} + \frac{T_{\text{remaining}}}{N_{\text{active}}(S)} \right\rceil\right)$$

Where:
- $P$ = Student's 1-based position in queue $S$.
- $T_{\text{avg}}(S)$ = **Dynamic Exponential Moving Average (EMA)** of service $S$:
  $$T_{\text{new}} = (0.30 \times T_{\text{actual}}) + (0.70 \times T_{\text{prev}})$$
  When a counter finishes a transaction, the actual duration updates this moving average in C++.
- $N_{\text{active}}(S)$ = Number of online counters currently eligible to handle service $S$.
- $T_{\text{remaining}}$ = Sum of remaining service time of currently busy counters handling service $S$.

---

### Feature D: No-Show & Cancellation Management
- **Grace Period (Configurable, default 5 mins):** When a token is called to a counter, a timer records the call timestamp. If the student does not arrive within the grace period, the counter flags **[Grace Expired]**, allowing the operator to declare a **No-Show**.
- **No Queue Blocking:** When marked No-Show or Cancelled, the token is freed immediately in C++, allowing the counter to dispatch the next student without delay.
- **Audit Logging:** Every no-show or cancellation logs the timestamp, reason, and operator name into `data/completed_records.csv` and memory.

---

### Feature E: Intelligent Workload Balancing
When an operator calls for the next student:
1. C++ inspects all service queues supported by that counter.
2. It prioritizes **Priority tokens** (special needs, faculty referral).
3. Among equal priorities, it applies **Earliest Arrival Time (strict FIFO fairness across services)**.
4. If multiple counters are available, the token routes to the counter with the **lowest projected workload**:
   $$\text{Workload}(C) = T_{\text{elapsed\_rem}} + \sum_{s \in \text{Supported}} \frac{\text{QueueCount}(s) \times T_{\text{avg}}(s)}{\text{SharingCounters}(s)}$$
5. **Manual Override:** Operators can explicitly summon any specific Token ID (e.g. for urgent queries), which is recorded with `[Manual Override]` in the audit trail.

---

# 3. DEEP-DIVE CODE WALKTHROUGH

Here is the exact explanation of how every C++ file works:

### A. `Token` Class (`src/Token.h` & `src/Token.cpp`)
- **Purpose:** Represents the visitor entity.
- **Key Members:**
  - `id`: Unique integer (101, 102...)
  - `serviceId`: "ADM", "FEE", "EXM", "CRT"
  - `studentName`, `studentId`, `priority` (0 or 1)
  - `status`: Enum (`WAITING`, `CALLED`, `SERVING`, `COMPLETED`, `CANCELLED`, `NOSHOW`)
  - `arrivalTime`, `callTime`, `startTime`, `finishTime` (`std::time_t`)
  - `assignedCounterId`, `counterName`, `notes`
- **Method `getFormattedId()`:** Formats integer `101` as `"ADM-101"` using `std::ostringstream`.
- **Method `toJson()`:** Serializes the token attributes directly into JSON format for the web dashboard.

---

### B. `ServiceType` Class (`src/ServiceType.h`)
- **Purpose:** Encapsulates service definitions and dynamic speed metrics.
- **Key Method `recordCompletedService(double actualMinutes)`:**
  Implements the Exponential Moving Average:
  `currentAvgDurationMinutes = (0.30 * actualMinutes) + (0.70 * currentAvgDurationMinutes)`.
  This allows wait estimates to adapt dynamically if transactions are moving faster or slower today.

---

### C. `ServiceQueue` Class (`src/ServiceQueue.h` & `src/ServiceQueue.cpp`)
- **Purpose:** Implements a custom **Singly Linked-List FIFO Queue**.
- **Structure:**
  ```cpp
  struct QueueNode {
      Token data;
      QueueNode* next;
      QueueNode(const Token& t) : data(t), next(nullptr) {}
  };
  ```
- **Method `enqueue(Token)`:**
  Creates a new node using `new QueueNode(token)`. If `rearNode == nullptr`, sets `frontNode = rearNode = newNode`. Otherwise, `rearNode->next = newNode; rearNode = newNode;`. Takes **$O(1)$ constant time**.
- **Method `dequeue(Token& outToken)`:**
  Saves `frontNode`, copies data, advances `frontNode = frontNode->next`, and calls `delete temp` to free heap memory. Takes **$O(1)$ constant time**.
- **Method `removeToken(int tokenId, Token& out)`:**
  Traverses the linked list to find and remove a specific token (used for voluntary student cancellation or operator manual override). Takes $O(n)$ time.
- **Destructor `~ServiceQueue()`:**
  Calls `clear()`, which iterates through all remaining nodes and calls `delete` to prevent memory leaks.

---

### D. `Counter` Class (`src/Counter.h` & `src/Counter.cpp`)
- **Purpose:** Represents a service counter desk.
- **Key Members:**
  - `id`, `name`, `operatorName`, `supportedServiceIds`
  - `status`: Enum (`COUNTER_AVAILABLE`, `COUNTER_SERVING`, `COUNTER_PAUSED`, `COUNTER_OFFLINE`)
  - `currentToken`, `hasActiveToken`, `callTimestamp`
  - Statistics: `totalServedCount`, `totalNoShowCount`, `totalCancelledCount`
- **Method `assignToken(Token)`:** Moves counter to `COUNTER_SERVING`, logs start time.
- **Method `completeCurrent(Token&)`:** Records finish time, increments `totalServedCount`, frees counter to `COUNTER_AVAILABLE`.
- **Method `markNoShow(Token&, reason)`:** Marks token `STATUS_NOSHOW`, frees counter to `COUNTER_AVAILABLE`.

---

### E. `QueueEngine` Class (`src/QueueEngine.h` & `src/QueueEngine.cpp`)
- **Purpose:** Central coordinator and brain of the system.
- **Key Collections:**
  - `std::map<std::string, ServiceType> services`: Map of all 4 services.
  - `std::map<std::string, ServiceQueue> queues`: Map of FIFO queues per service.
  - `std::vector<Counter> counters`: The 4 active service counters.
  - `std::vector<Token> auditHistory`: Persistent transaction logs.
- **Method `callNextForCounter(int counterId, Token& out)`:**
  The Intelligent Workload Balancing algorithm:
  - Scans all queues supported by this counter.
  - Looks at `frontNode` of each candidate queue.
  - Prioritizes `priority > 0` tokens first.
  - Among equal priorities, selects the token with the earliest `arrivalTime`.
  - Atomically dequeues and assigns to counter.
- **Method `saveState()` & `loadState()`:**
  Reads/writes `data/token_config.txt`, `data/active_queue.txt`, and exports `data/completed_records.csv` to ensure persistence across computer reboots.

---

### F. `HttpServer` Class (`src/HttpServer.h` & `src/HttpServer.cpp`)
- **Purpose:** Native Windows Winsock2 HTTP REST API Server.
- **How it Works:**
  - Initializes Winsock (`WSAStartup`).
  - Binds to port `8080` and listens for incoming TCP client connections.
  - Spawns a background worker thread via Windows API `CreateThread`.
  - Protects `QueueEngine` method calls with a Windows `CRITICAL_SECTION` lock (`EnterCriticalSection` / `LeaveCriticalSection`) to ensure thread safety.
  - Handles HTTP GET and POST requests:
    - `/api/status` &rarr; Full system state JSON.
    - `/api/token/generate` &rarr; Issues token.
    - `/api/counter/call` &rarr; Dispatches student to counter.
    - `/api/counter/complete` &rarr; Marks service complete.
    - `/api/counter/noshow` &rarr; Marks student as no-show.
    - `/api/counter/status` &rarr; Pauses or activates counter.
    - `/api/token/search` &rarr; Queries position and wait time.
  - Serves static assets (`index.html`, `favicon.svg`, `404.html`) with proper MIME types.

---

### G. `main.cpp`
- **Purpose:** Entry point for the executable.
- Starts the `HttpServer` on port `8080`.
- Provides an interactive, menu-driven Console CLI so examiners can evaluate the program either via terminal or via web browser.

---

# 4. TIME AND SPACE COMPLEXITY ANALYSIS

| Operation | C++ Function | Time Complexity | Space Complexity | Explanation |
|:---|:---|:---:|:---:|:---|
| **Add Student to Queue** | `ServiceQueue::enqueue` | **$O(1)$ Constant** | $O(1)$ | Directly links node at `rearNode->next` |
| **Call Next Student** | `ServiceQueue::dequeue` | **$O(1)$ Constant** | $O(1)$ | Directly advances `frontNode = frontNode->next` |
| **Inspect Front Student** | `ServiceQueue::peek` | **$O(1)$ Constant** | $O(1)$ | Reads `frontNode->data` |
| **Search Student in Line** | `ServiceQueue::findPosition` | **$O(n)$ Linear** | $O(1)$ | Traverses linked list of size $n$ |
| **Cancel Token in Queue** | `ServiceQueue::removeToken` | **$O(n)$ Linear** | $O(1)$ | Unlinks node and calls `delete` |
| **Workload Balancing** | `callNextForCounter` | **$O(S)$** where $S \le 4$ | $O(1)$ | Peeks front of supported service queues |
| **Wait Time Estimation** | `calculateEstimatedWait` | **$O(C)$** where $C \le 4$ | $O(1)$ | Scans online counters to sum capacity |

---

# 5. WHAT INTERVIEWERS WILL ASK & WHAT YOU SHOULD REPLY

### Q1: What is the biggest upgrade in your system compared to a basic queue?
**Your Reply:**
> *"Sir/Ma'am, traditional queue systems use a single queue with static wait times. Our system introduces three major upgrades:*  
> *1. **Multiple Service Queues:** Separate FIFO linked-list queues for Admission, Fees, Examination, and Certificates.*  
> *2. **Dynamic Moving-Average Estimation:** Instead of hardcoded guesses, our C++ backend recalculates wait times dynamically using exponential moving averages from actual service durations.*  
> *3. **Intelligent Workload Balancing:** Multi-service counters automatically pull the longest-waiting eligible student with priority handling, and no-shows are managed with configurable grace periods."*

---

### Q2: Why did you implement the backend in C++ instead of Node.js or Python?
**Your Reply:**
> *"Because C++ provides deterministic memory management, high execution speed, and direct pointer control. For real-time counter hardware and queue kiosks in banks or colleges, C++ ensures zero garbage-collection pauses. We built our own native Winsock HTTP server directly in C++ to serve JSON endpoints to the dashboard."*

---

### Q3: Why did you build a custom Linked List instead of an Array or `std::vector`?
**Your Reply:**
> *"An array has a fixed capacity that can overflow, and removing the front item requires shifting all remaining elements to the left in $O(n)$ time. With our Linked List queue, memory is allocated dynamically on demand, and both `enqueue` (at the rear) and `dequeue` (at the front) run in strictly **$O(1)$ constant time**."*

---

### Q4: How does your Workload Balancing algorithm work?
**Your Reply:**
> *"When a counter operator clicks 'Call Next', our C++ engine examines all service queues that counter is authorized to handle. It first checks for any Priority tokens. If none exist, it selects the token with the earliest arrival timestamp across all eligible queues, ensuring strict FIFO fairness across multiple desks."*

---

### Q5: How do you handle students who do not show up when called?
**Your Reply:**
> *"We implement a configurable Grace Period (default 5 minutes). When a token is called, C++ records the timestamp. If the student fails to arrive within the grace window, the counter flags 'Grace Expired', enabling the operator to mark them as No-Show. The token is immediately unassigned, freeing the counter without holding up the rest of the queue."*

---

### Q6: How do you prevent data loss if the computer turns off?
**Your Reply:**
> *"We use C++ file streams (`std::ifstream` and `std::ofstream`). Every token generation and state transition writes to `data/token_config.txt`, `data/active_queue.txt`, and `data/completed_records.csv`. When the software restarts, `loadState()` restores the queue order and resumes token numbers seamlessly."*

---

# 6. 2-MINUTE LIVE DEMONSTRATION SPEAKING SCRIPT

Use this script during your final presentation:

> *"Good morning respected examiners and faculty members.*
>
> *Today, I am demonstrating our college engineering project: the **Smart Queue and Waiting-Time Management System** with a complete **C++ core engine**.*
>
> *In university administrative buildings and clinics, standing in physical lines causes confusion, fatigue, and line-cutting. Our software completely digitizes this process.*
>
> *(Point to the screen)*  
> *Here is our application running. The backend is a native C++ executable that operates on port 8080 and hosts this clean dashboard.*
>
> *Our system provides five primary features:*
> *1. **Multiple Service Queues:** As you can see, we have separate FIFO queues for Admission, Fees, Examination, and Certificates.*
> *2. **Multiple Service Counters:** We have four counters running with individual operators, supported service specializations, and real-time status transitions.*
> *3. **Dynamic Wait Time Estimation:** When a student enters their roll number and gets a token, C++ calculates their estimated wait time using actual historical service duration and available counter throughput.*
> *4. **Intelligent Workload Balancing:** When Counter 1 clicks 'Call Next', C++ dispatches the earliest arrival from its supported queues, triggering an audible voice chime.*
> *5. **No-Show and Cancellation Management:** If a student does not arrive within the 5-minute grace period, the operator marks them as No-Show, freeing the counter without blocking the queue.*
>
> *All transactions are permanently audited to CSV. Thank you, and I am ready for your questions."*
