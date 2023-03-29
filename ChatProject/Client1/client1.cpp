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


#define MAX_SIZE 1024
using std::cout;
using std::cin;
using std::endl;
using std::string;
SOCKET client_sock;
string my_nick;

using namespace std;


const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 
const string username = "project"; // 데이터베이스 사용자 
const string password = "1234"; // 데이터베이스 접속 비밀번호 

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;


int chat_recv() {
	char buf[MAX_SIZE] = {}; //메시지 입력, 출력 위함
	string msg;
	while (1) {
		ZeroMemory(&buf, MAX_SIZE); //size만큼 0으로 채워준다 
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
	
	//로그인 

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

	string find_id, find_pw;
	string input_id, input_pw;
	bool check_id = 1, check_pw = 1;

	while (check_id == 1 || check_pw == 1) {

		check_id = 1, check_pw = 1;

		cout << "아이디를 입력하세요 -> \n";
		cin >> input_id;
		cout << "비밀번호를 입력하세요 -> \n";
		cin >> input_pw;

		// ID 확인 -> check_id == 0;
		con->setSchema("chatprogram");
		pstmt = con->prepareStatement("SELECT id FROM information;");
		result = pstmt->executeQuery();

		while (result->next()) {
			find_id = result->getString("id");
			if (find_id == input_id) {
				check_id = 0;
			}
		}
		cout << check_id << endl;
		
		// PW 확인 -> check_pw == 0;
		con->setSchema("chatprogram");
		pstmt = con->prepareStatement("SELECT pw FROM information;");
		result = pstmt->executeQuery();

		while (result->next()) {
			find_pw = result->getString("pw");
			if (find_pw == input_pw) {
				check_pw = 0;
			}
		}
		cout << check_pw << endl;
		
		if (check_id == 1) {
			cout << "잘못된 ID입니다." << endl << "다시 입력하세요" << endl;
		}
		if (check_pw == 1) {
			cout << "잘못된 PW입니다." << endl << "다시 입력하세요" << endl;
		}
		
	}
	
	
	//con->setSchema("chatprogram");
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

			con->setSchema("chatprogram");
			pstmt = con->prepareStatement("SELECT nick_name FROM information WHERE id = ?;");
			pstmt->setString(1, input_id);
			result = pstmt->executeQuery();

			while (result->next()) {
				input_nick = result->getString("nick_name");
			}

			cout << "닉네임은" << input_nick << "입니다." << endl;

			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
				//connect()는 정상연결 되었을 때 0 반환
				cout << "Server Connect" << endl;
				send(client_sock, input_nick.c_str(), input_nick.length(), 0);
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

	delete result;
	delete pstmt;
	delete con;
	system("pause");

	return 0;
	
}
