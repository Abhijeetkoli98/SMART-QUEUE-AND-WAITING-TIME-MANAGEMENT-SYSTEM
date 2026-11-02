#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <winsock2.h>
#include <windows.h>
#include <string>
#include "QueueEngine.h"

class HttpServer {
private:
    int port;
    std::string webRootDirectory;
    QueueEngine* engine;
    SOCKET listenSocket;
    bool running;
    HANDLE serverThread;
    CRITICAL_SECTION engineLock;

    static DWORD WINAPI ServerThreadRoutine(LPVOID lpParam);
    void runServerLoop();
    void handleClient(SOCKET clientSocket);

    // Helpers
    std::string getMimeType(const std::string& path);
    std::string readEntireFile(const std::string& fullPath, bool& success);
    std::string extractJsonValue(const std::string& json, const std::string& key);

public:
    HttpServer(int port, const std::string& webRootDir, QueueEngine* eng);
    ~HttpServer();

    bool start();
    void stop();
    bool isRunning() const { return running; }
    int getPort() const { return port; }
};

#endif // HTTP_SERVER_H

