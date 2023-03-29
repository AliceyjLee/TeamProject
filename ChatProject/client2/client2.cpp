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

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "project"; // 데이터베이스 사용자
const string password = "1234"; // 데이터베이스 접속 비밀번호


int chat_recv() {



	char buf[MAX_SIZE] = {}; //메시지 입력, 출력 위함
	string msg;
	while (1) {
		ZeroMemory(&buf, MAX_SIZE); 
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {// 0: 정상종료 >0:값이 잘 들어감
			msg = buf;
			string user;
			std::stringstream ss(msg); // stringstream = (slice해줌)-> user랑 masage분리
			ss >> user;

			//cout << buf << endl; 
			// 전체가 보이면 헷갈리기 때문에 내 닉네임이 아닌 경우에만 출력
			if (user != my_nick) cout << buf << endl; //같은 닉네임이 생성되지 않게 
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
	///////////////// 이전기록 출력////////////////////////////////////
	string find_user, find_msg;

	cout << "server의 기록을 출력합니다" << endl;
	con->setSchema("chatprogram");
	pstmt = con->prepareStatement("SELECT user_nick_name,user_message FROM message;");
	result = pstmt->executeQuery();

	while (result->next()) {
		find_user = result->getString("user_nick_name");
		find_msg = result->getString("user_message");
		cout << find_user << " : " << find_msg << endl;
	}
	cout << "------------------------------------------" << endl;
	///////////////// 로그인 ////////////////////////////////////
	string find_id, find_pw;
	string  input_id, input_pw;
	bool check_id = 0, check_pw = 0;


	////////////////////////////////////////////////////소켓통신  (여기서 메세지 가져와야할듯)

	con->setSchema("chatprogram");
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	string input_nick;
	if (!code) {
		//cout << "사용할 닉네임 입력 >>";
		//cin >> my_nick;
		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET; //주소체계정의
		client_addr.sin_port = htons(7777); //포트할당
		InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
		//client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		while (1) {
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
				//connect()는 정상연결 되었을 때 0 반환
				cout << "Server Connect" << endl;
				send(client_sock, my_nick.c_str(), my_nick.length(), 0);
				break;
				//닉네임을 서버로 보내주기
			}
			cout << "connecting..." << endl;
		}
		std::thread th2(chat_recv);

		while (1) { //채팅내용(문자열)받아서 보내주기
			string text;
			std::getline(cin, text);
			const char* buffer = text.c_str();
			send(client_sock, buffer, strlen(buffer), 0);
		}
		th2.join();
		closesocket(client_sock); //리소스 반환 
	}

	WSACleanup();
	return 0;
}
