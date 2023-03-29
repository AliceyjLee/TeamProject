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
#include <cppconn/statement.h>

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
	con->setSchema("chatprogram");
	stmt = con->createStatement();
	stmt->execute("set names euckr"); // �ѱ� ���ڵ��� ����
	if (stmt) { delete stmt; stmt = nullptr; }

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
	con->setSchema("chatprogram");

	//string find_id, find_pw;
	string input_id, input_pw;
	bool check_id = 1, check_pw = 1;

	while (check_id == 1 || check_pw == 1) {

		check_id = 1, check_pw = 1;

		cout << "���̵� �Է��ϼ��� -> \n";
		cin >> input_id;
		cout << "��й�ȣ�� �Է��ϼ��� -> \n";
		cin >> input_pw;

		// ID Ȯ�� -> check_id == 0;
		con->setSchema("chatprogram");
		pstmt = con->prepareStatement("SELECT id FROM information;");
		result = pstmt->executeQuery();

		while (result->next()) {
			//find_id = result->getString("id");
			if (result->getString("id") == input_id) {
				check_id = 0;
			}
		}
		cout << check_id << endl;

		// PW Ȯ�� -> check_pw == 0;
		con->setSchema("chatprogram");
		pstmt = con->prepareStatement("SELECT pw FROM information;");
		result = pstmt->executeQuery();

		while (result->next()) {
			//find_pw = result->getString("pw");
			if (result->getString("pw") == input_pw) {
				check_pw = 0;
			}
		}
		cout << check_pw << endl;

		if (check_id == 1) {
			cout << "�߸��� ID�Դϴ�." << endl << "�ٽ� �Է��ϼ���" << endl;
		}
		if (check_pw == 1) {
			cout << "�߸��� PW�Դϴ�." << endl << "�ٽ� �Է��ϼ���" << endl;
		}

	}


	////////////////////////////////////////////////////�������  
	
	//con->setSchema("chatprogram");
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

			con->setSchema("chatprogram");
			pstmt = con->prepareStatement("SELECT nick_name FROM information WHERE id = ?;");
			pstmt->setString(1, input_id);
			result = pstmt->executeQuery();

			while (result->next()) {
				input_nick = result->getString("nick_name");
			}

			cout << "�г�����" << input_nick << "�Դϴ�." << endl;

			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
				//connect()�� ���󿬰� �Ǿ��� �� 0 ��ȯ
				cout << "Server Connect" << endl;
				send(client_sock, input_nick.c_str(), input_nick.length(), 0);
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

	delete result;
	delete pstmt;
	delete con;
	system("pause");

	return 0;
}
