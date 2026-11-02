# Verification & Test Cases
## Smart Queue & Waiting-Time Management System (Genuine C++ Core)

This test suite was executed against the native C++ executable (`smart_queue.exe`) running on Windows 11 with MinGW GCC 6.3.0 and Winsock2 on `http://localhost:3000`.

---

### Test Summary Table

| Test ID | Test Scenario | Input Data / Endpoint | Expected Result | Actual Result (Verified) | Status |
|:---:|:---|:---|:---|:---|:---:|
| **TC-01** | **Server Initialization & TCP Bind** | Start `smart_queue.exe 3000 --daemon` | Winsock initializes socket, binds to `0.0.0.0:3000`, listens for clients | TCP Port 3000 active, server thread running | **PASS** |
| **TC-02** | **Static Web Asset Delivery** | `GET http://localhost:3000/web_gui/index.html` | Delivers dashboard with `Content-Type: text/html` | HTTP 200 OK, delivered 53,163 bytes | **PASS** |
| **TC-03** | **System State Snapshot** | `GET /api/status` | JSON response containing 4 services, 4 counters, and queue counts | Returned JSON with 4 counters, 4 services, status 200 | **PASS** |
| **TC-04** | **Multi-Queue Token Generation** | `POST /api/token/generate`<br>`{ serviceId: "ADM", studentName: "Aarav Sharma" }` | Enqueues token in Admission linked list, issues `ADM-112` | Token `ADM-112` created with timestamp and added to ADM queue | **PASS** |
| **TC-05** | **Priority Token Handling** | `POST /api/token/generate`<br>`{ serviceId: "EXM", priority: 1 }` | Token created with priority flag `1` (`EXM-114`) | Priority recorded in C++ `Token::priority` | **PASS** |
| **TC-06** | **Dynamic Wait-Time Calculation** | `POST /api/token/search`<br>`{ tokenId: 112 }` | Calculates estimated wait based on queue position and active counters | Returned `Position: #4, Est Wait: ~12 mins` | **PASS** |
| **TC-07** | **Intelligent Workload Balancing** | `POST /api/counter/call`<br>`{ counterId: 1 }` | Assigns highest-priority earliest-arrival compatible token | Dispatched `ADM-107` to Counter 1 | **PASS** |
| **TC-08** | **Service Completion & Moving Average** | `POST /api/counter/complete`<br>`{ counterId: 1 }` | Marks token `COMPLETED`, records actual duration, logs to CSV | Service duration calculated, counter freed to `AVAILABLE` | **PASS** |
| **TC-09** | **No-Show Management & Grace Period** | `POST /api/counter/noshow`<br>`{ counterId: 2, reason: "Absent after grace" }` | Marks token `NOSHOW`, logs reason, frees counter immediately | Token marked `NOSHOW`, counter freed without blocking line | **PASS** |
| **TC-10** | **Manual Override Assignment** | `POST /api/counter/override`<br>`{ counterId: 3, tokenId: 114 }` | Bypasses FIFO order, assigns specified token to Counter 3 | Token `EXM-114` assigned directly with `[Manual Override]` note | **PASS** |
| **TC-11** | **Counter State Transitions** | `POST /api/counter/status`<br>`{ counterId: 4, status: "PAUSED" }` | Transitions Counter 4 to `PAUSED`, excludes from dispatch | Counter 4 status set to `PAUSED` | **PASS** |
| **TC-12** | **Analytics & Audit Reporting** | `GET /api/history` and `GET /api/reports` | Returns aggregated metrics calculated in C++ | Returned `Completed: 1, No-Show: 1, Total: 2` | **PASS** |
| **TC-13** | **Data Persistence Across Restarts** | Restart `smart_queue.exe` | Restores next token ID and active waiting lines from `data/` | Next token continued without resetting to 101 | **PASS** |

---

### How to Reproduce These Tests on Your Computer

1. Open PowerShell or Command Prompt in the project folder:
   ```cmd
   cd "C:\Users\ABHIJEET\OneDrive\Desktop\field project"
   ```

2. Compile the C++ binary:
   ```cmd
   build.bat
   ```

3. Launch the server and open Google Chrome:
   ```cmd
   host_chrome.bat
   ```
   *(Or run `run.bat` for your default browser).*

4. Live Test Sequence in Browser (`http://localhost:3000`):
   - **Student Kiosk:** Enter your name and roll number, choose a service (e.g. Admission or Fee), and click **"Generate Token"**.
   - **Operator Counters:** Switch to the **Operator Counters** tab, select Counter 1 or 2, and click **"Call Next Student"**. Listen to the speech chime announce your token aloud.
   - **Service Completion:** Click **"Mark Service Completed"** to log the transaction to `data/completed_records.csv`.
   - **No-Show:** Call a token, wait or click **"Mark No-Show"** to observe counter unblocking.
   - **Audit Log:** Open the **Token History** tab to inspect the timestamped audit log.
