#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <stdlib.h> 
#include "mysql_connection.h" 
#include <cppconn/driver.h> 
#include <cppconn/exception.h> 
#include <cppconn/prepared_statement.h> 

#define MAX_SIZE 1024 
#define MAX_CLIENT 3
using std::cout;
using std::cin;
using std::endl;
using std::string;

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 
const string username = "project"; // 데이터베이스 사용자 
const string password = "1234"; // 데이터베이스 접속 비밀번호 

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;

struct SOCKET_INFO {
	SOCKET sck;
	string user;
};
std::vector<SOCKET_INFO> sck_list;
SOCKET_INFO server_sock;
int client_count = 0;

void server_init(); // 1. 소켓 초기화
void add_client(); // 2. 클라이언트 추가
void send_msg(const char* msg); //3. 클라이언트에게 msg 보내기
void recv_msg(int idx); // 4. 클라이언트에게 채팅 내용을 받음
void del_client(int idx); // 5. 소켓 닫아줌
void create_table();

int main() {
	try
	{
		driver = get_driver_instance();
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
	create_table();  //메시지_테이블 생성하고 시작하기 ////////////////////////여기 수정 

	if (!code) {
		server_init();

		std::thread th1[MAX_CLIENT];
		for (int i = 0; i < MAX_CLIENT; i++) {
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
		delete stmt;
		return 0;
	}
}

//1. 소켓 초기화
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

//2. 클라이언트 연결 & 추가
void add_client() {
	SOCKADDR_IN addr = {};

	int addrsize = sizeof(addr);
	char buf[MAX_SIZE] = {};

	ZeroMemory(&addr, addrsize);

	SOCKET_INFO new_client = {};


	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	recv(new_client.sck, buf, MAX_SIZE, 0);

	new_client.user = string(buf);


	string msg = "[공지]" + new_client.user + "님이 입장했습니다!";
	cout << msg << endl;

	sck_list.push_back(new_client);


	std::thread th(recv_msg, client_count);
	client_count++;

	cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
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

			////////////////////////////////메세지 저장부분 client로 옮기기
			pstmt = con->prepareStatement("INSERT INTO message(user_nick_name, user_message) VALUES(?,?)");
			pstmt->setString(1, sck_list[idx].user);
			pstmt->setString(2, string(buf));
			pstmt->execute();
			cout << "One row inserted." << endl;


			send_msg(msg.c_str());
		}
		else {
			msg = "[공지]" + sck_list[idx].user + "님이 퇴장했습니다.";
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

void create_table() {
	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}

	con->setSchema("chatprogram");

	// message 테이블 생성
	stmt = con->createStatement(); //회원가입 내역 저장 시 워크벤치에서 미리 테이블 생성하기
	stmt->execute("DROP TABLE IF EXISTS message");
	cout << "Finished dropping table (if existed)" << endl;
	stmt->execute("CREATE TABLE message (user_index serial PRIMARY KEY, user_nick_name VARCHAR(50), user_message VARCHAR(100));");
	cout << "Finished creating table" << endl;
}