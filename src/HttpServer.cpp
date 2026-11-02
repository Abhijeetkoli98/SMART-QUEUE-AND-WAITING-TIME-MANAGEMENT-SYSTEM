#include "HttpServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

HttpServer::HttpServer(int p, const std::string& webRootDir, QueueEngine* eng)
    : port(p), webRootDirectory(webRootDir), engine(eng),
      listenSocket(INVALID_SOCKET), running(false), serverThread(NULL) {
    InitializeCriticalSection(&engineLock);
}

HttpServer::~HttpServer() {
    stop();
    DeleteCriticalSection(&engineLock);
}

bool HttpServer::start() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "[C++ HttpServer] Failed to initialize Winsock.\n";
        return false;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "[C++ HttpServer] Failed to create socket.\n";
        WSACleanup();
        return false;
    }

    // Set reuseaddr
    int opt = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[C++ HttpServer] Bind failed on port " << port << ".\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    if (listen(listenSocket, 50) == SOCKET_ERROR) {
        std::cerr << "[C++ HttpServer] Listen failed.\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    running = true;
    serverThread = CreateThread(NULL, 0, ServerThreadRoutine, this, 0, NULL);
    if (!serverThread) {
        std::cerr << "[C++ HttpServer] Failed to create server thread.\n";
        closesocket(listenSocket);
        WSACleanup();
        running = false;
        return false;
    }

    std::cout << "[C++ HttpServer] Live native C++ backend running on http://localhost:" << port << "/\n";
    return true;
}

void HttpServer::stop() {
    if (running) {
        running = false;
        if (listenSocket != INVALID_SOCKET) {
            closesocket(listenSocket);
            listenSocket = INVALID_SOCKET;
        }
        if (serverThread) {
            WaitForSingleObject(serverThread, 2000);
            CloseHandle(serverThread);
            serverThread = NULL;
        }
        WSACleanup();
        std::cout << "[C++ HttpServer] Server stopped.\n";
    }
}

DWORD WINAPI HttpServer::ServerThreadRoutine(LPVOID lpParam) {
    HttpServer* self = static_cast<HttpServer*>(lpParam);
    self->runServerLoop();
    return 0;
}

void HttpServer::runServerLoop() {
    while (running) {
        sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            if (!running) break;
            continue;
        }

        handleClient(clientSocket);
        closesocket(clientSocket);
    }
}

std::string HttpServer::getMimeType(const std::string& path) {
    if (path.rfind(".html") != std::string::npos || path.rfind(".htm") != std::string::npos) return "text/html; charset=utf-8";
    if (path.rfind(".css") != std::string::npos) return "text/css; charset=utf-8";
    if (path.rfind(".js") != std::string::npos) return "application/javascript; charset=utf-8";
    if (path.rfind(".json") != std::string::npos) return "application/json; charset=utf-8";
    if (path.rfind(".svg") != std::string::npos) return "image/svg+xml";
    if (path.rfind(".png") != std::string::npos) return "image/png";
    if (path.rfind(".jpg") != std::string::npos || path.rfind(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.rfind(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain; charset=utf-8";
}

std::string HttpServer::readEntireFile(const std::string& fullPath, bool& success) {
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        success = false;
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    success = true;
    return ss.str();
}

std::string HttpServer::extractJsonValue(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t pos = json.find(needle);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";
    pos++;

    // Skip whitespace
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\r' || json[pos] == '\n')) {
        pos++;
    }

    if (pos >= json.size()) return "";

    // String value
    if (json[pos] == '"') {
        pos++;
        size_t endQuote = json.find('"', pos);
        if (endQuote == std::string::npos) return "";
        return json.substr(pos, endQuote - pos);
    }

    // Number or boolean value
    size_t endVal = json.find_first_of(",}\r\n ", pos);
    if (endVal == std::string::npos) endVal = json.size();
    return json.substr(pos, endVal - pos);
}

void HttpServer::handleClient(SOCKET clientSocket) {
    std::string request = "";
    while (true) {
        char buffer[4096];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break;
        buffer[bytesRead] = '\0';
        request.append(buffer, bytesRead);

        // Check if headers are complete
        size_t headerEnd = request.find("\r\n\r\n");
        size_t delimiterLen = 4;
        if (headerEnd == std::string::npos) {
            headerEnd = request.find("\n\n");
            delimiterLen = 2;
        }

        if (headerEnd != std::string::npos) {
            // Check for Content-Length
            size_t clPos = request.find("Content-Length:");
            if (clPos == std::string::npos) clPos = request.find("content-length:");

            if (clPos != std::string::npos && clPos < headerEnd) {
                size_t colon = request.find(':', clPos);
                size_t lineEnd = request.find('\n', colon);
                std::string clStr = request.substr(colon + 1, lineEnd - colon - 1);
                int expectedLength = std::atoi(clStr.c_str());

                int currentBodyLen = static_cast<int>(request.size() - (headerEnd + delimiterLen));
                if (currentBodyLen >= expectedLength) {
                    break; // Whole body received
                }
            } else {
                break; // No body or GET request
            }
        }
    }

    if (request.empty()) return;

    std::istringstream reqStream(request);
    std::string method, uri, version;
    reqStream >> method >> uri >> version;

    // Handle CORS preflight OPTIONS
    if (method == "OPTIONS") {
        std::string res = "HTTP/1.1 204 No Content\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n"
                          "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
                          "Content-Length: 0\r\n"
                          "Connection: close\r\n\r\n";
        send(clientSocket, res.c_str(), static_cast<int>(res.size()), 0);
        return;
    }

    // Locate body for POST requests
    std::string body = "";
    size_t bodyPos = request.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        body = request.substr(bodyPos + 4);
    } else {
        bodyPos = request.find("\n\n");
        if (bodyPos != std::string::npos) {
            body = request.substr(bodyPos + 2);
        }
    }

    // Strip query string from URI
    std::string cleanUri = uri;
    size_t qPos = cleanUri.find('?');
    if (qPos != std::string::npos) cleanUri = cleanUri.substr(0, qPos);

    // =========================================================================
    // REST API ENDPOINTS
    // =========================================================================
    if (cleanUri.rfind("/api/", 0) == 0) {
        std::string jsonResponse = "{}";
        int statusCode = 200;

        EnterCriticalSection(&engineLock);

        if (cleanUri == "/api/status" && method == "GET") {
            jsonResponse = engine->getFullStatusJson();
        }
        else if (cleanUri == "/api/history" && method == "GET") {
            jsonResponse = engine->getHistoryJson();
        }
        else if (cleanUri == "/api/reports" && method == "GET") {
            jsonResponse = engine->getReportsJson();
        }
        else if (cleanUri == "/api/qrcode" && method == "GET") {
            std::string url = engine->getQrCodeTargetUrl();
            std::string svg = engine->getQrCodeSvg();
            std::string uri = engine->getQrCodeDataUri();

            // Escape svg for JSON string
            std::string escSvg = "";
            for (size_t i = 0; i < svg.size(); ++i) {
                if (svg[i] == '"') escSvg += "\\\"";
                else if (svg[i] == '\n') escSvg += "\\n";
                else if (svg[i] == '\r') continue;
                else escSvg += svg[i];
            }
            jsonResponse = "{\"success\":true,\"url\":\"" + url + "\",\"svg\":\"" + escSvg + "\",\"dataUri\":\"" + uri + "\"}";
        }
        else if (cleanUri == "/api/qrcode.svg" && method == "GET") {
            std::string svg = engine->getQrCodeSvg();
            LeaveCriticalSection(&engineLock);

            std::ostringstream resStream;
            resStream << "HTTP/1.1 200 OK\r\n"
                      << "Content-Type: image/svg+xml; charset=utf-8\r\n"
                      << "Access-Control-Allow-Origin: *\r\n"
                      << "Content-Disposition: attachment; filename=\"student_kiosk_qr.svg\"\r\n"
                      << "Content-Length: " << svg.size() << "\r\n"
                      << "Connection: close\r\n\r\n"
                      << svg;
            std::string fullRes = resStream.str();
            send(clientSocket, fullRes.c_str(), static_cast<int>(fullRes.size()), 0);
            return;
        }
        else if (cleanUri == "/api/token/generate" && method == "POST") {
            std::string sId = extractJsonValue(body, "serviceId");
            std::string sName = extractJsonValue(body, "studentName");
            std::string stId = extractJsonValue(body, "studentId");
            std::string pStr = extractJsonValue(body, "priority");
            int prio = pStr.empty() ? 0 : std::atoi(pStr.c_str());

            if (sName.empty()) sName = "Student";
            if (sId.empty()) sId = "ADM";

            Token t = engine->generateToken(sId, sName, stId, prio);
            jsonResponse = "{\"success\":true,\"token\":" + t.toJson() + "}";
        }
        else if (cleanUri == "/api/token/search" && method == "POST") {
            std::string idStr = extractJsonValue(body, "tokenId");
            int id = std::atoi(idStr.c_str());

            Token t;
            int pos = -1;
            double wait = 0.0;
            std::string msg;
            bool found = engine->searchToken(id, t, pos, wait, msg);

            std::ostringstream ss;
            ss << "{\"found\":" << (found ? "true" : "false") << ","
               << "\"message\":\"" << msg << "\","
               << "\"position\":" << pos << ","
               << "\"estimatedWaitMinutes\":" << wait << ",";
            if (found) ss << "\"token\":" << t.toJson();
            else ss << "\"token\":null";
            ss << "}";
            jsonResponse = ss.str();
        }
        else if (cleanUri == "/api/token/cancel" && method == "POST") {
            std::string idStr = extractJsonValue(body, "tokenId");
            std::string reason = extractJsonValue(body, "reason");
            int id = std::atoi(idStr.c_str());

            Token cancelled;
            bool ok = engine->cancelTokenInQueue(id, reason, cancelled);
            jsonResponse = "{\"success\":" + std::string(ok ? "true" : "false") + "}";
        }
        else if (cleanUri == "/api/counter/call" && method == "POST") {
            std::string cIdStr = extractJsonValue(body, "counterId");
            int cId = std::atoi(cIdStr.c_str());

            Token called;
            bool ok = engine->callNextForCounter(cId, called);
            if (ok) {
                jsonResponse = "{\"success\":true,\"calledToken\":" + called.toJson() + "}";
            } else {
                jsonResponse = "{\"success\":false,\"message\":\"Queue empty or counter not eligible.\"}";
            }
        }
        else if (cleanUri == "/api/counter/override" && method == "POST") {
            std::string cIdStr = extractJsonValue(body, "counterId");
            std::string tIdStr = extractJsonValue(body, "tokenId");
            int cId = std::atoi(cIdStr.c_str());
            int tId = std::atoi(tIdStr.c_str());

            Token called;
            bool ok = engine->manualOverrideCall(cId, tId, called);
            if (ok) {
                jsonResponse = "{\"success\":true,\"calledToken\":" + called.toJson() + "}";
            } else {
                jsonResponse = "{\"success\":false,\"message\":\"Override failed. Counter may be busy or token not found.\"}";
            }
        }
        else if (cleanUri == "/api/counter/complete" && method == "POST") {
            std::string cIdStr = extractJsonValue(body, "counterId");
            int cId = std::atoi(cIdStr.c_str());

            Token completed;
            bool ok = engine->completeServiceAtCounter(cId, completed);
            if (ok) {
                jsonResponse = "{\"success\":true,\"completedToken\":" + completed.toJson() + "}";
            } else {
                jsonResponse = "{\"success\":false,\"message\":\"No active service in progress.\"}";
            }
        }
        else if (cleanUri == "/api/counter/noshow" && method == "POST") {
            std::string cIdStr = extractJsonValue(body, "counterId");
            std::string reason = extractJsonValue(body, "reason");
            int cId = std::atoi(cIdStr.c_str());

            Token noshow;
            bool ok = engine->markCounterNoShow(cId, reason, noshow);
            jsonResponse = "{\"success\":" + std::string(ok ? "true" : "false") + "}";
        }
        else if (cleanUri == "/api/counter/cancel" && method == "POST") {
            std::string cIdStr = extractJsonValue(body, "counterId");
            std::string reason = extractJsonValue(body, "reason");
            int cId = std::atoi(cIdStr.c_str());

            Token cancelled;
            bool ok = engine->markCounterCancelled(cId, reason, cancelled);
            jsonResponse = "{\"success\":" + std::string(ok ? "true" : "false") + "}";
        }
        else if (cleanUri == "/api/counter/status" && method == "POST") {
            std::string cIdStr = extractJsonValue(body, "counterId");
            std::string stStr = extractJsonValue(body, "status");
            int cId = std::atoi(cIdStr.c_str());

            CounterStatus st = COUNTER_AVAILABLE;
            if (stStr == "PAUSED") st = COUNTER_PAUSED;
            else if (stStr == "OFFLINE") st = COUNTER_OFFLINE;
            else if (stStr == "AVAILABLE") st = COUNTER_AVAILABLE;

            bool ok = engine->setCounterStatus(cId, st);
            jsonResponse = "{\"success\":" + std::string(ok ? "true" : "false") + "}";
        }
        else if (cleanUri == "/api/settings" && method == "POST") {
            std::string graceStr = extractJsonValue(body, "gracePeriodMinutes");
            if (!graceStr.empty()) {
                engine->setGracePeriodMinutes(std::atoi(graceStr.c_str()));
            }
            std::string qrTarget = extractJsonValue(body, "qrCodeTargetUrl");
            if (!qrTarget.empty()) {
                engine->setQrCodeTargetUrl(qrTarget);
            }
            jsonResponse = "{\"success\":true}";
        }
        else if (cleanUri == "/api/demo/load" && method == "POST") {
            engine->loadCollegeDemoData();
            jsonResponse = "{\"success\":true}";
        }
        else if (cleanUri == "/api/demo/reset" && method == "POST") {
            engine->resetAllQueues();
            jsonResponse = "{\"success\":true}";
        }
        else {
            statusCode = 404;
            jsonResponse = "{\"error\":\"Endpoint not found\"}";
        }

        LeaveCriticalSection(&engineLock);

        std::ostringstream resStream;
        resStream << "HTTP/1.1 " << statusCode << " OK\r\n"
                  << "Content-Type: application/json; charset=utf-8\r\n"
                  << "Access-Control-Allow-Origin: *\r\n"
                  << "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n"
                  << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
                  << "Content-Length: " << jsonResponse.size() << "\r\n"
                  << "Connection: close\r\n\r\n"
                  << jsonResponse;

        std::string fullRes = resStream.str();
        int totalSent = 0;
        int toSend = static_cast<int>(fullRes.size());
        while (totalSent < toSend) {
            int sent = send(clientSocket, fullRes.c_str() + totalSent, toSend - totalSent, 0);
            if (sent <= 0) break;
            totalSent += sent;
        }
        return;
    }

    // =========================================================================
    // STATIC FILE SERVING
    // =========================================================================
    std::string filePath = cleanUri;
    if (filePath == "/" || filePath == "/index.html") {
        filePath = "/web_gui/index.html";
    }

    std::string localDiskPath = webRootDirectory + filePath;
    // Normalize path slashes
    for (size_t i = 0; i < localDiskPath.size(); ++i) {
        if (localDiskPath[i] == '/') localDiskPath[i] = '\\';
    }

    bool fileFound = false;
    std::string fileContent = readEntireFile(localDiskPath, fileFound);

    if (!fileFound) {
        // Try falling back to 404.html
        std::string notFoundPath = webRootDirectory + "\\web_gui\\404.html";
        fileContent = readEntireFile(notFoundPath, fileFound);
        if (!fileFound) {
            fileContent = "<h1>404 Not Found</h1>";
        }
        std::ostringstream resStream;
        resStream << "HTTP/1.1 404 Not Found\r\n"
                  << "Content-Type: text/html; charset=utf-8\r\n"
                  << "Content-Length: " << fileContent.size() << "\r\n"
                  << "Connection: close\r\n\r\n"
                  << fileContent;
        std::string fullRes = resStream.str();
        int totalSent = 0;
        int toSend = static_cast<int>(fullRes.size());
        while (totalSent < toSend) {
            int sent = send(clientSocket, fullRes.c_str() + totalSent, toSend - totalSent, 0);
            if (sent <= 0) break;
            totalSent += sent;
        }
        return;
    }

    std::string mime = getMimeType(localDiskPath);
    std::ostringstream resStream;
    resStream << "HTTP/1.1 200 OK\r\n"
              << "Content-Type: " << mime << "\r\n"
              << "Access-Control-Allow-Origin: *\r\n"
              << "Content-Length: " << fileContent.size() << "\r\n"
              << "Connection: close\r\n\r\n"
              << fileContent;

    std::string fullRes = resStream.str();
    int totalSent = 0;
    int toSend = static_cast<int>(fullRes.size());
    while (totalSent < toSend) {
        int sent = send(clientSocket, fullRes.c_str() + totalSent, toSend - totalSent, 0);
        if (sent <= 0) break;
        totalSent += sent;
    }
}

