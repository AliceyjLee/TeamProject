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
		ZeroMemory(&buf, MAX_SIZE); 
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
	///////////////// ������� ���////////////////////////////////////
	string find_user, find_msg;

	cout << "server�� ����� ����մϴ�" << endl;
	con->setSchema("chatprogram");
	pstmt = con->prepareStatement("SELECT user_nick_name,user_message FROM message;");
	result = pstmt->executeQuery();

	while (result->next()) {
		find_user = result->getString("user_nick_name");
		find_msg = result->getString("user_message");
		cout << find_user << " : " << find_msg << endl;
	}
	cout << "------------------------------------------" << endl;
	///////////////// �α��� ////////////////////////////////////
	string find_id, find_pw;
	string  input_id, input_pw;
	bool check_id = 0, check_pw = 0;


	////////////////////////////////////////////////////�������  (���⼭ �޼��� �����;��ҵ�)

	con->setSchema("chatprogram");
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	string input_nick;
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
