#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <WinSock2.h>
#define MAX_SIZE 1024 
#define MAX_CLIENT 3
using std::cout;
using std::cin;
using std::endl;
using std::string;

struct SOCKET_INFO {
	SOCKET sck;
	string user;
};
std::vector<SOCKET_INFO> sck_list;
SOCKET_INFO server_sock;
int client_count = 0;

void server_init(); // 1. ���� �ʱ�ȭ
void add_client(); // 2. Ŭ���̾�Ʈ �߰�
void send_msg(const char* msg); //3. Ŭ���̾�Ʈ���� msg ������
void recv_msg(int idx); // 4. Ŭ���̾�Ʈ���� ä�� ������ ����
void del_client(int idx); // 5. ���� �ݾ���


int main() {
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (!code) {
		server_init();

		std::thread th1[MAX_CLIENT];
		for (int i = 0; i < MAX_CLIENT; i++) {
			//3���� cmdâ�� ������ ��? (���� client3���� �����ϰ� MAX_CLIENT�� 2�� �θ�?)
			th1[i] = std::thread(add_client);
		}

		while (1) {
			string text, msg = "";
			std::getline(cin, text);
			const char* buf = text.c_str();

			msg = server_sock.user + " : " + buf;
			send_msg(msg.c_str());
		}
		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i].join();
		}
		closesocket(server_sock.sck);
		WSACleanup();
		return 0;
	}
}

//1. ���� �ʱ�ȭ
void server_init() {
	server_sock.sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(7777);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	// localhost = 127.0.0.1

	bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));


	listen(server_sock.sck, SOMAXCONN);
	server_sock.user = "server";

	cout << "server On!" << endl;
}

//2. Ŭ���̾�Ʈ ���� & �߰�
void add_client() {
	SOCKADDR_IN addr = {};

	int addrsize = sizeof(addr);
	char buf[MAX_SIZE] = {};

	ZeroMemory(&addr, addrsize);

	SOCKET_INFO new_client = {};


	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	recv(new_client.sck, buf, MAX_SIZE, 0);

	new_client.user = string(buf);


	string msg = "[����]" + new_client.user + "���� �����߽��ϴ�!";
	cout << msg << endl;

	sck_list.push_back(new_client);

	std::thread th(recv_msg, client_count);
	client_count++;
	cout << "[����] ���� ������ �� : " << client_count << "��" << endl;
	send_msg(msg.c_str());

	th.join();

}


void send_msg(const char* msg) {
	for (int i = 0; i < client_count; i++) {
		send(sck_list[i].sck, msg, MAX_SIZE, 0);
	}
}

void recv_msg(int idx) {
	char buf[MAX_SIZE] = {};
	string msg = "";
	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
			msg = sck_list[idx].user + " : " + buf;
			cout << msg << endl;
			send_msg(msg.c_str());
		}
		else {
			msg = "[����]" + sck_list[idx].user + "���� �����߽��ϴ�.";
			cout << msg << endl;
			send_msg(msg.c_str());
			del_client(idx);
			return;
		}
	}
}
void del_client(int idx) {
	closesocket(sck_list[idx].sck);
	client_count--;
}

