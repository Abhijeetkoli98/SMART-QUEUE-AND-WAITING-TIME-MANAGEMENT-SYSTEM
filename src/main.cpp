#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include "QueueEngine.h"
#include "HttpServer.h"

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void pauseConsole() {
    std::cout << "\n  Press Enter to return to menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        } else {
            std::cout << "  [ERROR] Invalid input! Please enter a valid number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string getStringInput(const std::string& prompt) {
    std::string text;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, text);
        if (!text.empty()) return text;
        std::cout << "  [ERROR] Input cannot be empty. Please enter again.\n";
    }
}

void printBanner(int port) {
    std::cout << "===============================================================================\n";
    std::cout << "     SMART QUEUE & WAITING-TIME MANAGEMENT SYSTEM (GENUINE C++ CORE)          \n";
    std::cout << "       Department of Computer Engineering - College Major Project             \n";
    std::cout << "       Live Native C++ Web Server active at: http://localhost:" << port << "/   \n";
    std::cout << "===============================================================================\n";
}

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc >= 2) {
        port = std::atoi(argv[1]);
        if (port <= 1024) port = 8080;
    }

    QueueEngine engine("data");
    HttpServer server(port, ".", &engine);

    if (!server.start()) {
        std::cerr << "  [WARNING] Could not start HTTP server on port " << port << ". Falling back to console only.\n";
    }

    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printBanner(server.getPort());

        // Quick Dashboard Summary
        std::cout << "\n  SYSTEM SNAPSHOT:\n";
        std::cout << "  +---------------------------------------------------------------------------+\n";
        std::cout << "  | Total Waiting: " << std::left << std::setw(4) << engine.getTotalWaitingCount()
                  << " | Available Counters: " << std::setw(4) << engine.getAvailableCounterCount()
                  << " | Overall Avg Wait: " << std::fixed << std::setprecision(1) << engine.getOverallAvgWaitMinutes() << " mins    |\n";
        std::cout << "  +---------------------------------------------------------------------------+\n";

        // Show Counters
        std::cout << "  ACTIVE COUNTERS:\n";
        const std::vector<Counter>& counters = engine.getCounters();
        for (size_t i = 0; i < counters.size(); ++i) {
            std::cout << "  [" << counters[i].getId() << "] "
                      << std::left << std::setw(32) << counters[i].getName()
                      << " Status: " << std::setw(11) << counters[i].getStatusString();
            if (counters[i].getHasActiveToken()) {
                std::cout << "Serving: " << counters[i].getCurrentToken().getFormattedId()
                          << " (" << counters[i].getCurrentToken().getStudentName() << ")"
                          << " [" << std::fixed << std::setprecision(1) << counters[i].getElapsedServiceMinutes() << "m]";
            } else {
                std::cout << "Idle";
            }
            std::cout << "\n";
        }

        std::cout << "\n  MAIN MENU:\n";
        std::cout << "  [1] Generate New Student Token (Select Service: ADM, FEE, EXM, CRT)\n";
        std::cout << "  [2] View All Service Queues (FIFO Order per Service)\n";
        std::cout << "  [3] Call Next Student at Counter (Intelligent Workload Balancing)\n";
        std::cout << "  [4] Mark Current Service Completed at Counter\n";
        std::cout << "  [5] Mark No-Show or Cancel at Counter (Grace Period Aware)\n";
        std::cout << "  [6] Manual Override: Assign Specific Token to Counter\n";
        std::cout << "  [7] Search Token & Calculate Dynamic Wait Time\n";
        std::cout << "  [8] Counter Status Management (Available / Paused / Offline)\n";
        std::cout << "  [9] Settings & Grace Period Configuration\n";
        std::cout << "  [10] Load College Demo Sample Data\n";
        std::cout << "  [11] Open Web Dashboard in Browser\n";
        std::cout << "  [0] Exit Application\n";
        std::cout << "  -----------------------------------------------------------------------------\n";

        choice = getIntInput("  Enter your choice [0-11]: ");

        switch (choice) {
            case 1: { // Generate Token
                std::cout << "\n  --- GENERATE STUDENT TOKEN ---\n";
                std::cout << "  Available Services:\n";
                std::cout << "    [1] ADM - Admission & Registration (Avg ~8m)\n";
                std::cout << "    [2] FEE - Fees & Financial Accounts (Avg ~4m)\n";
                std::cout << "    [3] EXM - Examination & Results (Avg ~6m)\n";
                std::cout << "    [4] CRT - Transcripts & Certificates (Avg ~5m)\n";
                int svcChoice = getIntInput("  Select service [1-4]: ");

                std::string sId = "ADM";
                if (svcChoice == 2) sId = "FEE";
                else if (svcChoice == 3) sId = "EXM";
                else if (svcChoice == 4) sId = "CRT";

                std::string name = getStringInput("  Enter Student Name: ");
                std::string stId = getStringInput("  Enter Student Roll/ID (e.g. SE-2026-042): ");
                int prio = getIntInput("  Is Priority? (1 = Yes / Faculty/Special, 0 = Normal): ");

                Token t = engine.generateToken(sId, name, stId, prio);
                int wait = static_cast<int>(engine.calculateEstimatedWaitMinutes(sId, engine.getWaitingCountForService(sId)));

                std::cout << "\n  [SUCCESS] Token Generated in C++ Core!\n";
                std::cout << "  Token ID        : " << t.getFormattedId() << "\n";
                std::cout << "  Service         : " << t.getServiceName() << "\n";
                std::cout << "  Student Name    : " << t.getStudentName() << "\n";
                std::cout << "  Queue Position  : #" << engine.getWaitingCountForService(sId) << " in " << sId << "\n";
                std::cout << "  Est. Wait Time  : ~" << wait << " minutes\n";
                pauseConsole();
                break;
            }

            case 2: { // View All Queues
                std::cout << "\n  --- ALL LIVE SERVICE QUEUES (STRICT FIFO ORDER) ---\n";
                const char* svcCodes[] = { "ADM", "FEE", "EXM", "CRT" };
                for (int s = 0; s < 4; ++s) {
                    std::cout << "\n  === Service: " << svcCodes[s] << " (Total Waiting: "
                              << engine.getWaitingCountForService(svcCodes[s]) << ") ===\n";
                    // Search in engine
                    int count = engine.getWaitingCountForService(svcCodes[s]);
                    if (count == 0) {
                        std::cout << "    (Queue is currently empty)\n";
                    } else {
                        std::cout << "    Pos   Token ID     Student Name         Roll / ID       Arrival   Wait Est\n";
                        std::cout << "    ------------------------------------------------------------------------\n";
                        // Find positions
                        for (int id = 100; id < 300; ++id) {
                            Token tok;
                            int pos;
                            double w;
                            std::string msg;
                            if (engine.searchToken(id, tok, pos, w, msg) && pos > 0 && tok.getServiceId() == svcCodes[s]) {
                                std::cout << "    #" << std::left << std::setw(5) << pos
                                          << std::setw(13) << tok.getFormattedId()
                                          << std::setw(21) << tok.getStudentName()
                                          << std::setw(16) << tok.getStudentId()
                                          << std::setw(10) << tok.getArrivalTimeString()
                                          << "~" << static_cast<int>(w) << " mins\n";
                            }
                        }
                    }
                }
                pauseConsole();
                break;
            }

            case 3: { // Call Next
                std::cout << "\n  --- CALL NEXT STUDENT (INTELLIGENT WORKLOAD BALANCING) ---\n";
                int cId = getIntInput("  Enter Counter ID to dispatch (1-4): ");
                Token called;
                if (engine.callNextForCounter(cId, called)) {
                    std::cout << "\n  [DISPATCHED] Assigned " << called.getFormattedId()
                              << " (" << called.getStudentName() << ") for ["
                              << called.getServiceName() << "] to Counter " << cId << "!\n";
                } else {
                    std::cout << "  [INFO] Cannot dispatch. Either queue is empty, or counter is currently busy/offline.\n";
                }
                pauseConsole();
                break;
            }

            case 4: { // Complete Service
                std::cout << "\n  --- COMPLETE SERVICE AT COUNTER ---\n";
                int cId = getIntInput("  Enter Counter ID (1-4): ");
                Token completed;
                if (engine.completeServiceAtCounter(cId, completed)) {
                    std::cout << "\n  [SUCCESS] Service marked COMPLETED for "
                              << completed.getFormattedId() << " (" << completed.getStudentName() << ").\n";
                    std::cout << "  Service Duration: " << std::fixed << std::setprecision(1)
                              << completed.getActualServiceDurationMinutes() << " mins.\n";
                    std::cout << "  Dynamic average service time updated in C++ engine and logged to CSV.\n";
                } else {
                    std::cout << "  [INFO] No active service in progress at Counter " << cId << ".\n";
                }
                pauseConsole();
                break;
            }

            case 5: { // No-Show or Cancel
                std::cout << "\n  --- MARK NO-SHOW OR CANCEL AT COUNTER ---\n";
                int cId = getIntInput("  Enter Counter ID (1-4): ");
                Counter* c = engine.getCounter(cId);
                if (!c || !c->getHasActiveToken()) {
                    std::cout << "  [INFO] Counter " << cId << " has no active token.\n";
                    pauseConsole();
                    break;
                }

                std::cout << "  Current Token: " << c->getCurrentToken().getFormattedId()
                          << " (" << c->getCurrentToken().getStudentName() << ")\n";
                std::cout << "  Elapsed Time : " << std::fixed << std::setprecision(1) << c->getElapsedServiceMinutes() << " minutes.\n";
                std::cout << "  Grace Period : " << engine.getGracePeriodMinutes() << " minutes.\n";
                if (engine.isGracePeriodExpired(cId)) {
                    std::cout << "  [ALERT] Grace period HAS EXPIRED. Eligible for No-Show.\n";
                } else {
                    std::cout << "  [NOTE] Grace period is still active ("
                              << (engine.getGracePeriodMinutes() - c->getElapsedServiceMinutes()) << "m remaining).\n";
                }

                std::cout << "  Action: [1] Mark No-Show   [2] Cancel Service   [0] Abort: ";
                int act = getIntInput("");
                if (act == 1) {
                    std::string reason = getStringInput("  Enter reason for No-Show: ");
                    Token tok;
                    engine.markCounterNoShow(cId, reason, tok);
                    std::cout << "  [MARKED] Token " << tok.getFormattedId() << " marked as NO-SHOW.\n";
                } else if (act == 2) {
                    std::string reason = getStringInput("  Enter reason for Cancellation: ");
                    Token tok;
                    engine.markCounterCancelled(cId, reason, tok);
                    std::cout << "  [CANCELLED] Token " << tok.getFormattedId() << " marked as CANCELLED.\n";
                }
                pauseConsole();
                break;
            }

            case 6: { // Manual Override
                std::cout << "\n  --- MANUAL OVERRIDE: CALL SPECIFIC TOKEN ---\n";
                int cId = getIntInput("  Enter Target Counter ID (1-4): ");
                int tId = getIntInput("  Enter Numeric Token ID to call (e.g. 103): ");

                Token called;
                if (engine.manualOverrideCall(cId, tId, called)) {
                    std::cout << "  [SUCCESS] Manual Override: Token " << called.getFormattedId()
                              << " assigned directly to Counter " << cId << "!\n";
                } else {
                    std::cout << "  [FAILED] Override failed. Counter may be busy or token ID not found in waiting lines.\n";
                }
                pauseConsole();
                break;
            }

            case 7: { // Search Token
                std::cout << "\n  --- SEARCH TOKEN & ESTIMATE WAITING TIME ---\n";
                int tId = getIntInput("  Enter numeric Token ID (e.g. 101): ");
                Token found;
                int pos;
                double wait;
                std::string msg;
                if (engine.searchToken(tId, found, pos, wait, msg)) {
                    std::cout << "\n  [FOUND] Token Details:\n";
                    std::cout << "  Token Number  : " << found.getFormattedId() << "\n";
                    std::cout << "  Student Name  : " << found.getStudentName() << "\n";
                    std::cout << "  Student ID    : " << found.getStudentId() << "\n";
                    std::cout << "  Service       : " << found.getServiceName() << "\n";
                    std::cout << "  Status        : " << found.getStatusString() << "\n";
                    if (pos == 0) {
                        std::cout << "  Location      : Currently at " << found.getCounterName() << "\n";
                    } else if (pos > 0) {
                        std::cout << "  Position      : #" << pos << " in queue (" << (pos - 1) << " ahead)\n";
                        std::cout << "  Est. Wait     : ~" << static_cast<int>(wait) << " minutes\n";
                    }
                    std::cout << "  System Message: " << msg << "\n";
                } else {
                    std::cout << "  [NOT FOUND] " << msg << "\n";
                }
                pauseConsole();
                break;
            }

            case 8: { // Counter Status Management
                std::cout << "\n  --- COUNTER STATUS MANAGEMENT ---\n";
                int cId = getIntInput("  Select Counter ID (1-4): ");
                std::cout << "  Select Status: [1] AVAILABLE   [2] PAUSED (Break)   [3] OFFLINE: ";
                int sChoice = getIntInput("");
                CounterStatus st = COUNTER_AVAILABLE;
                if (sChoice == 2) st = COUNTER_PAUSED;
                else if (sChoice == 3) st = COUNTER_OFFLINE;

                engine.setCounterStatus(cId, st);
                std::cout << "  [UPDATED] Counter " << cId << " status updated!\n";
                pauseConsole();
                break;
            }

            case 9: { // Settings
                std::cout << "\n  --- SETTINGS & CONFIGURATION ---\n";
                std::cout << "  Current Grace Period: " << engine.getGracePeriodMinutes() << " minutes.\n";
                int newGrace = getIntInput("  Enter new grace period in minutes (1-30): ");
                if (newGrace >= 1 && newGrace <= 30) {
                    engine.setGracePeriodMinutes(newGrace);
                    std::cout << "  [SUCCESS] Grace period updated to " << newGrace << " minutes.\n";
                }
                pauseConsole();
                break;
            }

            case 10: { // Load College Demo Data
                std::cout << "\n  Loading realistic college demonstration data (8 tokens across ADM, FEE, EXM, CRT)...\n";
                engine.loadCollegeDemoData();
                std::cout << "  [SUCCESS] College demo sample data loaded!\n";
                pauseConsole();
                break;
            }

            case 11: { // Open Web Dashboard
                std::cout << "\n  Opening Smart Queue Visual Web Dashboard in browser...\n";
                std::string url = "http://localhost:" + std::to_string(server.getPort()) + "/web_gui/index.html";
#ifdef _WIN32
                std::string cmd = "start " + url;
                std::system(cmd.c_str());
#endif
                pauseConsole();
                break;
            }

            case 0: { // Exit
                std::cout << "\n  Saving all queue states and shutting down C++ engine...\n";
                break;
            }

            default: {
                std::cout << "  [ERROR] Invalid choice!\n";
                pauseConsole();
                break;
            }
        }
    }

    server.stop();
    std::cout << "  Application cleanly terminated. Thank you!\n";
    return 0;
}
