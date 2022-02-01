#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>

#pragma warning(disable: 4996)

SOCKET Connections[100];
int Counter = 0;

enum Packet {
	P_ChatMessage,
	P_Test,
	P_Array,
	P_Symbol
};

void SendMessage(const char* msg, int msg_size, Packet packet, int index ) {
	for (int i = 0; i < Counter; i++) {
		if (i == index) {
			Packet msgtype = P_ChatMessage;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[i], msg, msg_size, NULL);
		}
	}
};

bool ProcessPacket(int index, Packet packettype) {
	switch (packettype) {
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);
		std::cout << msg;
		for (int i = 0; i < Counter; i++) {
			if (i == index) {
				Packet msgtype = P_ChatMessage;
				send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
				send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
				send(Connections[i], msg, msg_size, NULL);
			}
		}
		delete[] msg;
		break;
	}
	case P_Array:
	{
		int array_size;
		recv(Connections[index], (char*)&array_size, sizeof(int), NULL);
		int* arr = new int[array_size];
		int bytesReceived = recv(Connections[index], (char*)arr, (array_size * sizeof(int)), NULL);
		if (bytesReceived == -1)
		{
			std::string msg = "Error in recv(). Quitting";
			std::cout << msg << std::endl;
			SendMessage(msg.c_str(), msg.size(), P_ChatMessage, index);
		}
		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
		}
		std::ofstream fout("array.txt", std::ios::out | std::ios::binary);
		for (int i = 0; i < array_size; i++)
		{
			std::cout << arr[i] << " ";
			//fout.write((char*)&arr[i], sizeof(array_size));
			fout << arr[i] << " ";
		}
		fout << "\n" << array_size << " elements in array ";
		fout.close();
		std::cout << std::endl << "Data received and recorded!" << std::endl;
		std::string msg = "Server successfully received data!";
		SendMessage(msg.c_str(), msg.size(), P_ChatMessage, index);
		break;
	}
	case P_Symbol:
	{
		int g;
		recv(Connections[index], (char*)&g, sizeof(int), NULL);
		if (g == 1)
		{
			std::cout << "callg1";
			std::ifstream is("symbol.txt");
			if (!is)
			{
				std::cout << "file error open";
				return false;
			}
			char str[100];
			while (is)
			{
				is.getline(str, 100);
				std::cout<<"String: " << str <<" - sent to client  "<< std::endl;
				SendMessage(str, 100, P_ChatMessage, NULL);
			}
			is.close();
			std::cout << "call1 exit";
		}
	    else if(g == 2) {
			std::cout << "callg2";
			int msg_size;
			recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
			char* msg = new char[msg_size + 1];
			msg[msg_size] = '\0';
			recv(Connections[index], msg, msg_size, NULL);
			std::cout << msg;
			std:: fstream file;
			std::string filename;
			filename = "symbol.txt";

			file.open(filename.c_str());
			file.seekp(5);
			file.put(msg[0]);
			file.close();
			std::cout << "call2 exit";
			delete[] msg;
			break;
		}
		else
		{
			std::cout << "Error.(P_Symbol)";
			break;
		}
		break;
	}
	default:

		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;

	};

	return true;
	}


void ClientHandler(int index) 
{ 
	Packet packettype;
	while (true) {
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(index, packettype)) {
			break;
		}
	}
	closesocket(Connections[index]);
}

int main(int argc, char* argv[]) {
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

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			std::string msg = "The connection with the server has been established!";
			int msg_size = msg.size();
			Packet msgtype = P_ChatMessage;
			send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}


	system("pause");
	return 0;
}