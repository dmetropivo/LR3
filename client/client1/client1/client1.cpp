#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

SOCKET Connection;

enum  Packet {
	P_ChatMessage,
	P_Test,
	P_Array,
	P_Symbol
};

bool ProcessPacket(Packet packettype) {
	switch (packettype) {
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
		break;
	}
	case P_Test:
		std::cout << "Test packet.\n";
		break;
	default:
		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;
	}

	return true;
}

void ClientHandler() {
	Packet packettype;
	while (true) {
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype)) {
			break;
		}
	}
	closesocket(Connection);
}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	std::string msg1;
	while (true) {
		std::cout << std::endl << "Choose operation: \n 1 - CHAT \n 2 - Send array \n 3 - get string \n 4 - Insert symbol " << std::endl;
		int oper;
		std::cin >> oper;
		std::cout << std::endl;
		switch (oper) {
		case 1:
			std::cout << "Write (quit) to leave the chat" << std::endl;
			while (true)
			{
				std::cout << "Enter message >>  ";
				std::getline(std::cin, msg1);
				if (msg1 == "quit")
				{
					break;
				};
				int msg_size = msg1.size();
				Packet packettype = P_ChatMessage;
				send(Connection, (char*)&packettype, sizeof(Packet), NULL);
				send(Connection, (char*)&msg_size, sizeof(int), NULL);
				send(Connection, msg1.c_str(), msg_size, NULL);
				Sleep(10);

			}
			break;
		case 2:
		{std::cout << "Sending array..." << std::endl;
		Packet packettype = P_Array;
		int array_size = 20;
		int* arr = new int[array_size];
		std::cout << "Data array: ";
		for (int i = 0; i < array_size; i++)
		{
			arr[i] = i + 5;
			std::cout << arr[i] << " ";
		}
		std::cout << std::endl;
		std::cout << "Array size: " << array_size << std::endl;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		send(Connection, (char*)&array_size, sizeof(int), NULL);
		send(Connection, (char*)arr, (array_size * sizeof(int)), NULL);
		Sleep(10);
		delete[] arr;
		break;
		}
		case 3:
		{
			Packet packettype = P_Symbol;
			int msg_size;
			int g = 1;
			send(Connection, (char*)&packettype, sizeof(Packet), NULL);
			send(Connection, (char*)&g, sizeof(msg_size), NULL);
			break;
		}
		case 4:
		{
			Packet packettype = P_Symbol;
			int g = 2;
			int msg_size;
			send(Connection, (char*)&packettype, sizeof(Packet), NULL);
			send(Connection, (char*)&g, sizeof(msg_size), NULL);
			std::string symbol;
			//int msg_size;
			std::cout << "Enter character: ";
			std::cin >> symbol;
			msg_size = symbol.size();
			//send(Connection, (char*)&packettype, sizeof(Packet), NULL);
			send(Connection, (char*)&msg_size, sizeof(int), NULL);
			send(Connection, symbol.c_str(), sizeof(msg_size), NULL);
			Sleep(10);
			break;
		}
		}
	}

	system("pause");
	return 0;
}