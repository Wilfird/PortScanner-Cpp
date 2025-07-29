#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

std::mutex mtx;

void scanPort(const std::string& ip, int port) {
    SOCKET sock;
    sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) return;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    int result = connect(sock, (SOCKADDR*)&addr, sizeof(addr));
    if (result != SOCKET_ERROR) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[OPEN] Port " << port << std::endl;
    }

    closesocket(sock);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    std::string targetIP;
    std::cout << "Enter IP to scan (e.g., 127.0.0.1): ";
    std::cin >> targetIP;

    const int startPort = 1;
    const int endPort = 1024;

    std::vector<std::thread> threads;

    for (int port = startPort; port <= endPort; ++port) {
        threads.emplace_back(scanPort, targetIP, port);
        if (threads.size() >= 100) {
            for (auto& t : threads) t.join();
            threads.clear();
        }
    }

    // Join any remaining threads
    for (auto& t : threads) t.join();

    WSACleanup();
    return 0;
}
