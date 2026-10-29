# Verification & Test Cases
## Smart Queue and Waiting-Time Management System (C++)

This document details the test suite executed on the compiled `smart_queue.exe` executable on Windows 11 with MinGW GCC 6.3.0.

---

### Test Summary Table

| Test ID | Test Scenario | Input Data | Expected Result | Actual Result (Verified) | Status |
|:---:|:---|:---|:---|:---|:---:|
| **TC-01** | Generate New Token | Name: "Aarav Sharma"<br>Service: "Doc Verification" | System generates token `T-101`, arrival time stamped, wait time estimated | Generated `T-101`, added to queue, wait time ~0 mins | **PASS** |
| **TC-02** | FIFO Ordering Verification | Enqueue `T-101`, `T-102`, `T-103`, `T-104` | When viewing queue, order must be strictly T-101 at #1, T-102 at #2, etc. | Order displayed: #1 T-101, #2 T-102, #3 T-103, #4 T-104 | **PASS** |
| **TC-03** | Search Token in Queue | Search ID: `102` | Display token details, position `#2`, people ahead: `1`, estimated wait: `5 mins` | Displayed position `#2`, 1 ahead, est wait `~5 mins` | **PASS** |
| **TC-04** | Call Next Person | Option `3` (Call Next) | Front person (`T-101`) dequeued from waiting line and moved to counter | `T-101` moved to "Currently Serving", waiting count decreased by 1 | **PASS** |
| **TC-05** | Search Token Currently at Counter | Search ID: `101` | Position displayed as `0` ("At Counter"), estimated wait `0 minutes` | Displayed "At Counter (Currently Being Served)", 0 wait | **PASS** |
| **TC-06** | Complete Service & File Logging | Option `4` (Complete Service) | Marks `T-101` completed; appends record to `data/completed_records.csv` | Record appended with timestamp to `completed_records.csv` | **PASS** |
| **TC-07** | Search Non-Existent Token | Search ID: `999` | Show clean error: "Token not found in active queue" | Displayed clear error message without crashing | **PASS** |
| **TC-08** | Call Next from Empty Queue | Clear queue, then select Option `3` | Show informative message: "The waiting queue is empty" | Displayed "[INFO] Cannot call next person: The waiting queue is empty" | **PASS** |
| **TC-09** | Invalid User Input Handling | Enter non-integer `abc` at menu prompt | Clear input buffer, show error, reprompt cleanly | Displayed "[ERROR] Invalid input! Please enter a valid number" | **PASS** |
| **TC-10** | Persistence Across Restarts | Exit program and restart `smart_queue.exe` | Next token ID should remain `105`, active queue restored | Token numbering started at `105`, previous active tokens preserved | **PASS** |
| **TC-11** | Web UI Dashboard Rendering | Open `web_gui/index.html` in browser | Metrics cards, tables, and buttons render cleanly without horizontal overflow | Metrics cards, tables, and buttons rendered responsively | **PASS** |
| **TC-12** | Web UI Token Generation | Click "Generate Token", enter "Rohan Gupta" | Token T-105 created, added to queue table, waiting count increments | Generated T-105, queue updated to 4 waiting | **PASS** |
| **TC-13** | Web UI Search & Estimation | Search ID `103` in Web Modal | Displays Position `#2`, 1 ahead, `~5 mins` estimated wait time | Correctly displayed Pos #2, 1 ahead, 5m wait | **PASS** |
| **TC-14** | Web UI Call Next Person | Click "Call Next Person" button | Moves `T-102` to Currently Serving card, triggers speech voice announcement | Counter card updated to T-102, speech synthesized | **PASS** |
| **TC-15** | Web UI Prevent Double Call | Click "Call Next" when counter is busy | Displays error banner asking to complete current person | Blocked call, showed warning banner | **PASS** |
| **TC-16** | Web UI Mark Completed | Click "Complete Service" button | Clears serving counter and appends record to Completed History table | Counter reset to idle, record logged in table | **PASS** |
| **TC-17** | Web UI Search Missing Token | Search Token `999` in Web Modal | Displays red alert banner: "Token T-999 Not Found in Active Queue" | Showed clean error message | **PASS** |

---

### How to Reproduce These Tests on Your Computer

1. Open PowerShell or Command Prompt in the project folder:
   ```cmd
   cd "C:\Users\ABHIJEET\OneDrive\Desktop\field project"
   ```

2. Compile the project:
   ```cmd
   build.bat
   ```

3. Launch the application:
   ```cmd
   smart_queue.exe
   ```

4. Quick Test Sequence:
   - Press `8` then `Enter`: Loads 4 demo sample tokens (`T-101` to `T-104`).
   - Press `2` then `Enter`: View the FIFO queue table.
   - Press `5` then `Enter`, enter `102`: Checks wait time for token 102.
   - Press `3` then `Enter`: Calls token 101 to the service counter.
   - Press `4` then `Enter`: Marks token 101 as completed.
   - Press `6` then `Enter`: Reads and displays `data/completed_records.csv`.
   - Press `0` then `Enter`: Exits cleanly while saving all states.

