# Smart Queue and Waiting-Time Management System (C++)

> **College Project:** Second-Year Computer Engineering (SE Computer)  
> **Topic:** Data Structures & Object-Oriented Programming (OOP) in C++  
> **Developed With:** Standard C++11, MinGW GCC, HTML5/CSS3/JS Web GUI Dashboard  
> **Live Web Demo:** [https://smart-queue-and-waiting-time-manage.vercel.app/web_gui/index.html](https://smart-queue-and-waiting-time-manage.vercel.app/web_gui/index.html)  
> **GitHub Repository:** [https://github.com/Abhijeetkoli98/SMART-QUEUE-AND-WAITING-TIME-MANAGEMENT-SYSTEM](https://github.com/Abhijeetkoli98/SMART-QUEUE-AND-WAITING-TIME-MANAGEMENT-SYSTEM)  

---

## 📌 Project Overview
The **Smart Queue and Waiting-Time Management System** is an automated desktop application designed to solve chaotic physical queues in banks, hospital clinics, college admission offices, and government service counters.

Instead of standing in long lines with no idea when they will be served:
1. Each person receives a unique token number (e.g., `T-101`, `T-102`).
2. Visitors are placed in a dynamic **Linked-List Queue** operating under strict **First-In, First-Out (FIFO)** discipline.
3. Visitors can query their token number anytime to see their exact position in line, the number of people ahead, and their estimated waiting time in minutes.
4. Counter operators can call the next person, mark services as completed, view history, and export records to a `CSV` file.
5. All system state (token counters and active queues) is automatically saved to local files, so restarting the application never loses data or resets token numbering.

---

## 📁 Project Folder Structure

```
field project/
│── build.bat                 <- One-click Windows script to compile C++ code
│── run.bat                   <- One-click Windows script to run the console app
│── open_gui.bat              <- One-click script to open the visual GUI dashboard
│── smart_queue.exe           <- Compiled standalone C++ executable
│── README.md                 <- Project documentation & setup guide
│
├── src/                      <- Complete C++ Source Code
│   ├── Token.h               <- Token class header (attributes & encapsulation)
│   ├── Token.cpp             <- Token class implementation
│   ├── Queue.h               <- Custom Linked-List Queue header (FIFO Node structure)
│   ├── Queue.cpp             <- O(1) Enqueue, Dequeue, Peek, Traverse logic
│   ├── QueueManager.h        <- Business logic, file I/O & wait-time formula
│   ├── QueueManager.cpp      <- Manager implementation & data persistence
│   └── main.cpp              <- Interactive console UI & menu system
│
├── data/                     <- Persistent Storage Files
│   ├── token_config.txt      <- Preserves next Token ID and average service time
│   ├── active_queue.txt      <- Preserves unserved active tokens across restarts
│   └── completed_records.csv <- Historical log of all completed visits (Excel compatible)
│
├── web_gui/                  <- Companion Visual GUI Dashboard
│   └── index.html            <- Modern, responsive dashboard (runs offline in any browser)
│
└── docs/                     <- College Submission Documentation
    ├── PROJECT_REPORT.md     <- Complete academic project report with problem statement & objectives
    ├── SYSTEM_ARCHITECTURE.md<- Mermaid UML Class, Use-Case & Flowchart diagrams
    ├── TEST_CASES.md         <- Verified test suite with inputs, expected and actual results
    └── PRESENTATION_AND_VIVA.md<- Slide outline, spoken presentation script, and Viva Q&A
```

---

## 🛠️ Software Requirements & Prerequisites

- **Operating System:** Windows 7/8/10/11
- **C++ Compiler:** MinGW GCC (`g++` version 6.3 or higher supporting C++11)
- **Editor (Optional):** VS Code, Dev-C++, Code::Blocks, or Notepad++
- **Browser (For GUI):** Google Chrome, Microsoft Edge, or Mozilla Firefox

---

## 🚀 How to Compile and Run

### Option 1: Using the Automated Scripts (Easiest)
1. **Compile:** Double-click `build.bat` (or run `.\build.bat` in PowerShell/CMD).
2. **Run Console App:** Double-click `run.bat` (or run `.\run.bat`).
3. **Open Visual GUI Dashboard:** Double-click `open_gui.bat` (or double-click `web_gui\index.html`).

### Option 2: Manual Compilation via Command Line
Open PowerShell or CMD in the project folder and run:
```cmd
g++ -std=c++11 -Wall -Wextra src/Token.cpp src/Queue.cpp src/QueueManager.cpp src/main.cpp -o smart_queue.exe
.\smart_queue.exe
```

---

## 📐 Mathematical Waiting-Time Formula

The estimated waiting time is calculated mathematically based on queue position and counter status:

$$\text{Estimated Wait Time (mins)} = (\text{Position} - 1) \times \text{Avg Service Time} + (\text{Active Counter Time})$$

- If Token `T-103` is at **Position #3**, there are $3 - 1 = 2$ people waiting ahead in line.
- If the average service duration is **5 minutes** per person and someone is currently being served at the counter:
  $$\text{Wait Time} = (2 \times 5) + (1 \times 5) = 15 \text{ minutes}$$

---

## 💡 Key Object-Oriented & Data Structure Concepts Used

1. **Custom Linked-List Queue:**
   - Instead of a fixed-size array, a singly linked list with `frontNode` and `rearNode` is used.
   - **$O(1)$ Enqueue:** Direct pointer update at `rearNode->next`.
   - **$O(1)$ Dequeue:** Direct pointer update at `frontNode = frontNode->next`.
   - Dynamic memory management: Uses `new` to create nodes and `delete` in the destructor to prevent memory leaks.
2. **Encapsulation:**
   - Private member variables (`tokenId`, `customerName`, `serviceType`) are protected from direct external modification and accessed through public getters and setters.
3. **Data Persistence:**
   - File handling using `<fstream>` ensures that token numbers do not restart when the application is reopened.

---

## 🎓 College Viva Preparation Highlights

1. **Why C++?**
   - High performance, deterministic memory control via pointers, and native support for Object-Oriented Programming without heavy virtual machine overhead.
2. **Why Linked-List Queue over std::queue or an Array?**
   - Shows examiners a clear understanding of dynamic pointer manipulation and avoids array size limits.
3. **What is FIFO?**
   - **First-In, First-Out:** The person arriving first is guaranteed to be served first.

