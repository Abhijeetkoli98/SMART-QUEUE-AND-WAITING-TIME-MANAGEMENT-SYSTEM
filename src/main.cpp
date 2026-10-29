#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include "QueueManager.h"

// Clear screen utility (cross-platform)
void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// Pause utility for user to read output
void pauseConsole() {
    std::cout << "\n  Press Enter to return to menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Safely read integer input
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

// Safely read non-empty line of text
std::string getStringInput(const std::string& prompt) {
    std::string text;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, text);
        if (!text.empty()) {
            return text;
        }
        std::cout << "  [ERROR] Input cannot be empty. Please enter again.\n";
    }
}

// Print standard application header
void printBanner() {
    std::cout << "==================================================================\n";
    std::cout << "      SMART QUEUE AND WAITING-TIME MANAGEMENT SYSTEM             \n";
    std::cout << "          Department of Computer Engineering - College Project     \n";
    std::cout << "==================================================================\n";
}

int main() {
    QueueManager manager("data");
    int choice = -1;

    while (choice != 0) {
        clearScreen();
        printBanner();
        manager.displayCurrentStatus();

        std::cout << "\n  MAIN MENU:\n";
        std::cout << "  [1] Generate New Token (Add Person to Queue)\n";
        std::cout << "  [2] View Current Waiting Queue (FIFO Order)\n";
        std::cout << "  [3] Call Next Person (Move to Service Counter)\n";
        std::cout << "  [4] Mark Current Service as Completed\n";
        std::cout << "  [5] Search Token & Estimate Waiting Time\n";
        std::cout << "  [6] View Completed Service Records (From File)\n";
        std::cout << "  [7] Configure Average Service Time (Minutes)\n";
        std::cout << "  [8] Load Demo Sample Data (Quick Demonstration)\n";
        std::cout << "  [0] Exit Application\n";
        std::cout << "  ----------------------------------------------------------------\n";

        choice = getIntInput("  Enter your choice [0-8]: ");

        switch (choice) {
            case 1: { // Generate New Token
                std::cout << "\n  --- GENERATE NEW TOKEN ---\n";
                std::string name = getStringInput("  Enter Customer/Student Name : ");
                
                std::cout << "  Select Service Type:\n";
                std::cout << "    1. General Inquiry\n";
                std::cout << "    2. Document Verification\n";
                std::cout << "    3. Fee Payment / Billing\n";
                std::cout << "    4. Other Service\n";
                int svcChoice = getIntInput("    Select option [1-4]: ");
                
                std::string service;
                if (svcChoice == 1) service = "General Inquiry";
                else if (svcChoice == 2) service = "Doc Verification";
                else if (svcChoice == 3) service = "Fee Payment";
                else {
                    service = getStringInput("    Enter custom service name: ");
                }

                Token t = manager.generateToken(name, service);
                std::cout << "\n  [SUCCESS] Token Successfully Generated!\n";
                std::cout << "  -----------------------------------------------------\n";
                std::cout << "  Token Number  : " << t.getFormattedId() << "\n";
                std::cout << "  Name          : " << t.getName() << "\n";
                std::cout << "  Service       : " << t.getService() << "\n";
                std::cout << "  Arrival Time  : " << t.getArrivalTime() << "\n";
                int waitMins = manager.calculateWaitTime(manager.getTotalWaiting());
                std::cout << "  Est. Wait Time: ~" << waitMins << " minute(s)\n";
                std::cout << "  -----------------------------------------------------\n";
                pauseConsole();
                break;
            }

            case 2: { // View Queue
                std::cout << "\n  --- CURRENT WAITING QUEUE (FIFO ORDER) ---\n";
                manager.displayWaitingQueue();
                pauseConsole();
                break;
            }

            case 3: { // Call Next Person
                std::cout << "\n  --- CALL NEXT PERSON ---\n";
                if (manager.hasCurrentlyServing()) {
                    std::cout << "  [WARNING] A person is already being served: "
                              << manager.getCurrentlyServing().getFormattedId() << " ("
                              << manager.getCurrentlyServing().getName() << ")\n";
                    std::cout << "  Please complete their service (Option 4) before calling the next person.\n";
                } else {
                    Token calledToken;
                    if (manager.callNextPerson(calledToken)) {
                        std::cout << "  [SUCCESS] Calling next person to counter!\n";
                        std::cout << "  Now Serving : " << calledToken.getFormattedId() 
                                  << " - " << calledToken.getName()
                                  << " [" << calledToken.getService() << "]\n";
                    } else {
                        std::cout << "  [INFO] Cannot call next person: The waiting queue is empty.\n";
                    }
                }
                pauseConsole();
                break;
            }

            case 4: { // Mark Service Completed
                std::cout << "\n  --- COMPLETE SERVICE ---\n";
                Token completedToken;
                if (manager.completeCurrentService(completedToken)) {
                    std::cout << "  [SUCCESS] Service marked as COMPLETED for "
                              << completedToken.getFormattedId() << " ("
                              << completedToken.getName() << ")!\n";
                    std::cout << "  Record has been saved to 'data/completed_records.csv'.\n";
                } else {
                    std::cout << "  [INFO] No active service in progress to complete.\n";
                }
                pauseConsole();
                break;
            }

            case 5: { // Search Token & Estimate Wait Time
                std::cout << "\n  --- SEARCH TOKEN & ESTIMATE WAITING TIME ---\n";
                int searchId = getIntInput("  Enter numeric Token ID to search (e.g., 101): ");
                
                int position, estimatedWait;
                Token foundToken;
                if (manager.searchToken(searchId, position, foundToken, estimatedWait)) {
                    std::cout << "\n  [FOUND] Token Details:\n";
                    std::cout << "  -----------------------------------------------------\n";
                    std::cout << "  Token ID        : " << foundToken.getFormattedId() << "\n";
                    std::cout << "  Name            : " << foundToken.getName() << "\n";
                    std::cout << "  Service         : " << foundToken.getService() << "\n";
                    std::cout << "  Arrival Time    : " << foundToken.getArrivalTime() << "\n";
                    std::cout << "  Current Status  : " << foundToken.getStatus() << "\n";

                    if (position == 0) {
                        std::cout << "  Queue Position  : At Counter (Currently Being Served)\n";
                        std::cout << "  Est. Wait Time  : 0 minutes (You are at the counter!)\n";
                    } else {
                        std::cout << "  Queue Position  : #" << position << " in waiting line\n";
                        std::cout << "  People Ahead    : " << (position - 1) << " person(s)\n";
                        std::cout << "  Est. Wait Time  : ~" << estimatedWait << " minute(s)\n";
                        std::cout << "  Formula Used    : (" << (position - 1) << " ahead) * "
                                  << manager.getAvgServiceTime() << " mins/person"
                                  << (manager.hasCurrentlyServing() ? " + 1 person at counter" : "")
                                  << "\n";
                    }
                    std::cout << "  -----------------------------------------------------\n";
                } else {
                    std::cout << "  [ERROR] Token T-" << searchId << " not found in active queue.\n";
                    std::cout << "  (It may have already been served, or the token number does not exist)\n";
                }
                pauseConsole();
                break;
            }

            case 6: { // View Completed Records
                manager.displayCompletedRecords();
                pauseConsole();
                break;
            }

            case 7: { // Configure Average Service Time
                std::cout << "\n  --- CONFIGURE AVERAGE SERVICE TIME ---\n";
                std::cout << "  Current setting: " << manager.getAvgServiceTime() << " minutes per person.\n";
                int newTime = getIntInput("  Enter new average service time in minutes (1-60): ");
                if (newTime >= 1 && newTime <= 60) {
                    manager.setAvgServiceTime(newTime);
                    std::cout << "  [SUCCESS] Average service time updated to " << newTime << " minutes.\n";
                } else {
                    std::cout << "  [ERROR] Invalid duration. Please specify between 1 and 60 minutes.\n";
                }
                pauseConsole();
                break;
            }

            case 8: { // Load Demo Sample Data
                std::cout << "\n  --- LOAD DEMO SAMPLE DATA ---\n";
                std::cout << "  Generating 4 sample tokens for quick demonstration...\n";
                manager.generateToken("Aarav Sharma", "Doc Verification");
                manager.generateToken("Priya Patel", "Fee Payment");
                manager.generateToken("Rahul Verma", "General Inquiry");
                manager.generateToken("Sneha Kulkarni", "Fee Payment");
                std::cout << "  [SUCCESS] 4 sample tokens created and queued in FIFO order!\n";
                pauseConsole();
                break;
            }

            case 0: { // Exit
                std::cout << "\n  Saving queue state and exiting Smart Queue System.\n";
                std::cout << "  Thank you for using the system!\n";
                break;
            }

            default: {
                std::cout << "  [ERROR] Invalid choice! Please select an option between 0 and 8.\n";
                pauseConsole();
                break;
            }
        }
    }

    return 0;
}

