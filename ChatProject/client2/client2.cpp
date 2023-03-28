#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <WS2tcpip.h>
#include <stdlib.h> 
#include "mysql_connection.h" 
#include <cppconn/driver.h> 
#include <cppconn/exception.h> 
#include <cppconn/prepared_statement.h> 

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;

#define MAX_SIZE 1024
using std::cout;
using std::cin;
using std::endl;
using std::string;
SOCKET client_sock;
string my_nick;

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "project"; // �����ͺ��̽� �����
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ


int chat_recv() {
	char buf[MAX_SIZE] = {}; //�޽��� �Է�, ��� ����
	string msg;
	while (1) {
		ZeroMemory(&buf, MAX_SIZE); //size��ŭ 0���� ä���ش� 
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {// 0: �������� >0:���� �� ��
			msg = buf;
			string user;
			std::stringstream ss(msg); // stringstream = (slice����)-> user�� masage�и�
			ss >> user;

			//cout << buf << endl; 
			// ��ü�� ���̸� �򰥸��� ������ �� �г����� �ƴ� ��쿡�� ���
			if (user != my_nick) cout << buf << endl; //���� �г����� �������� �ʰ� 
		}
		else {
			cout << "Server Off!" << endl;
			return -1;
		}
	}
}
int main() {
	try
	{
		driver = get_driver_instance();
		//for demonstration only. never save password in the code!
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	std::string find_id, find_pw;
	std::string  input_id, input_pw;

	con->setSchema("chatprogram");

	//�α��� 
	while (true) {

		cout << "���̵� �Է��ϼ��� -> \n";
		cin >> input_id;
		cout << "��й�ȣ�� �Է��ϼ��� -> \n";
		cin >> input_pw;

		//void Login_client();


		pstmt = con->prepareStatement("SELECT * FROM information;");
		result = pstmt->executeQuery();
		cout << result << endl;
		while (result->next()) {
			find_id = result->getString("id");
			find_pw = result->getString("pw");
			cout << find_id << endl;
			cout << find_pw << endl;
		}

		/*
		while (result->next())
			printf("Reading from table=(%d, %s, %d)\n", result->getInt(1), result->getString(2).c_str(), result->getInt(3));
		*/



		//printf("Reading from table=(%d, %s, %d)\n", result->getInt(1), result->getString(2).c_str(), result->getInt(3));



		if (result->next()) {
			cout << "�α��� ����! \n";
			break;
		}
		else {
			cout << "������ ���ų� �߸��� ȸ�������� �Է��ϼ̽��ϴ�. \n";
			cout << "�ٽ� �õ��ϼ���. \n";
		}
	}
	//select  






	if (!code) {
		//cout << "����� �г��� �Է� >>";
		//cin >> my_nick;
		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET; //�ּ�ü������
		client_addr.sin_port = htons(7777); //��Ʈ�Ҵ�
		InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
		//client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		while (1) {
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
				//connect()�� ���󿬰� �Ǿ��� �� 0 ��ȯ
				cout << "Server Connect" << endl;
				send(client_sock, my_nick.c_str(), my_nick.length(), 0);
				break;
				//�г����� ������ �����ֱ�
			}
			cout << "connecting..." << endl;
		}
		std::thread th2(chat_recv);

		while (1) { //ä�ó���(���ڿ�)�޾Ƽ� �����ֱ�
			string text;
			std::getline(cin, text);
			const char* buffer = text.c_str();
			send(client_sock, buffer, strlen(buffer), 0);
		}
		th2.join();
		closesocket(client_sock); //���ҽ� ��ȯ 
	}

	WSACleanup();
	return 0;
}
