
/* Copyright (C) 2022 - CodeIndian Technologies  - All Rights Reserved
 * No part of this file should be copied, distributed or modified without
 * Proper appovals from the owner(s)
 */

 /* -----------------------Revision History------------------------------------------
 *
 * 8-Oct-2022 Satish - Licensing
 */

#include "LicenseManager.h"
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "json.hpp"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;


using namespace std;
LicenseManager::LicenseManager()
{
}

bool LicenseManager::isValidLicense()
{
    bool isValidLicense = false;
    try
    {
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed" << std::endl;
            return 1;
        }

        // Set the server's IP address and port number
        std::string serverIp = "192.168.29.10";
        int serverPort = 8080;

        // Create a TCP socket
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        // Connect to the server
        struct addrinfo* result = NULL;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        if (getaddrinfo(serverIp.c_str(), std::to_string(serverPort).c_str(), &hints, &result) != 0)
        {
            std::cerr << "Error resolving server address" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        if (connect(clientSocket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR)
        {
            std::cerr << "Error connecting to server: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        std::cout << "Connected to server " << serverIp << ":" << serverPort << std::endl;

        // Create the request model
        RequestModel request;
        request.HostName = getenv("COMPUTERNAME");
        request.IPAddress = GetLocalIPAddress();
        request.ProductName = "Auto Revit 2022";

        // Serialize the request model to JSON
        json jsonRequest = {
            {"HostName", request.HostName},
            {"IPAddress", request.IPAddress},
            {"ProductName", request.ProductName}
        };

        std::string jsonString = jsonRequest.dump();

        // Send the request to the server
        if (send(clientSocket, jsonString.c_str(), static_cast<int>(jsonString.length()), 0) == SOCKET_ERROR)
        {
            std::cerr << "Error sending request to server: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        std::cout << "Sent: " << jsonString << std::endl;

        // Receive the response from the server
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead == SOCKET_ERROR)
        {
            std::cerr << "Error receiving response from server: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        std::string jsonResponse(buffer, bytesRead);

        // Deserialize the response JSON to the response model
        json jsonResponseObj = json::parse(jsonResponse);

        ResponseModel responseObject;
        responseObject.ValidLicense = jsonResponseObj["ValidLicense"].get<bool>();

        // Process the response
        std::cout << "Received: License Status=" << std::boolalpha << responseObject.ValidLicense << std::endl;

        // Close the socket
        closesocket(clientSocket);

        // Cleanup Winsock
        WSACleanup();

        isValidLicense = responseObject.ValidLicense;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
    }
    return isValidLicense;
}

std::string LicenseManager::GetLocalIPAddress()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return "";
	}

	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) != 0)
	{
		return "";
	}

	struct addrinfo* result = NULL;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(hostName, NULL, &hints, &result) != 0)
	{
		return "";
	}

	char ip[INET_ADDRSTRLEN];
	sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(result->ai_addr);
	inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);

	freeaddrinfo(result);
	WSACleanup();
	return ip;
}

