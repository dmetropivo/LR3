#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define DEFAULT_PORT "27015"
int main()
{
	string buf = "";
	while (true)
	{
		cout << "Operation: \n";
		cout << "1 - Return the number of element of the string";
		cout << "2 - Increase all numbers in the file";
		int a;
		cin >> a;
		if (a == 1)
		{
			cout << "Enter the element: ";
			char el;
			cin >> el;
			buf = "1 "; buf += el;
			break;
		}
		else if (a == 2)
		{
			cout << "Enter a value on which all the numbers will be increased: ";
			string val;
			cin >> val;
			if (atoi(val.c_str()) == 0 && val[0] != '0')
				cout << "Incorrect! Try again!\n";
			else
			{
				buf = "2 "; buf += val;
				break;
			}
		}
		else if (a == 0)
			return 0;
		char buf1[512] = { 0 };
		WSADATA wd;
		SOCKET sock = INVALID_SOCKET;
		struct addrinfo* result = NULL, * ptr = NULL, hints;
		int res;
		res = WSAStartup(MAKEWORD(2, 2), &wd);
		if (res != 0)
			return 0;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		res = getaddrinfo("Coffee\0", DEFAULT_PORT, &hints, &result);
		if (res != 0)
			return 0;
		cout << "Waiting for server to the connection....\n";
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			sock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
			if (sock == INVALID_SOCKET)
				return 0;
			res = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (res == SOCKET_ERROR)
			{
				closesocket(sock);
				sock = INVALID_SOCKET;
				continue;
			}
			break;
		}
		freeaddrinfo(result);
		if (sock == INVALID_SOCKET)
			return 0;
		res = send(sock, buf.c_str(), buf.length(), 0);

		if (res == SOCKET_ERROR)
			return 0;
		do {
			res = recv(sock, buf1, 512, 0);
			if (res > 0)
				cout << "Answer received: " << buf1 << endl;
			else if (res == 0)
				cout << "Connection closed\n";
			else
				cout << "Something went wrong...\n";
		} while (res > 0);
		closesocket(sock);
		WSACleanup();
		return 0;
	}
}