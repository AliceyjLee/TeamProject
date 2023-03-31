#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <WS2tcpip.h>
#include <stdlib.h> 
#include <Windows.h> 
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

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "project"; // 데이터베이스 사용자
const string password = "1234"; // 데이터베이스 접속 비밀번호

void korean();
void recv_prev_msg();
void duplicate_login(string input, string query, bool check, string* create_input);


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
	korean(); // 한국어 인코딩

	///////////////// 로그인 ////////////////////////////////////
	con->setSchema("chatprogram");

	string input_id, input_pw;
	bool check_id = 1, check_pw = 1;

	duplicate_login("id", "SELECT id FROM information;", check_id, &input_id); // 로그인-아이디
	duplicate_login("pw", "SELECT pw FROM information;", check_pw, &input_pw); // 로그인-비번

	///////////////// 이전기록 출력////////////////////////////////////

	recv_prev_msg();

	////////////////////////////////////////////////////소켓통신  

	//con->setSchema("chatprogram");
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	string input_nick;

	if (!code) {

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


			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);   //글자 색 바꿔주는 함수 (빨간색)
				cout << "닉네임은" << input_nick << "입니다." << endl;
				cout << input_nick << "님의 채팅 시작" << endl;
				cout << "-------------------------------------------------" << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); 
				//cout << "Server Connect" << endl;
				send(client_sock, input_nick.c_str(), input_nick.length(), 0);

				break;
				//닉네임을 서버로 보내주기
			}
			cout << "connecting..." << endl;
		}

		std::thread th2(chat_recv);
		string find_nick;
		bool end_client = 0;  //회원이 탈퇴 했을 경우 

		while (1) { //채팅내용(문자열)받아서 보내주기
			///////////////채팅창을 켠 상태로 회원 탈퇴했을 경우
			///회원 찾아서 없으면 server에서 공지띄우고 count -- 하기

			con->setSchema("chatprogram");
			pstmt = con->prepareStatement("SELECT nick_name FROM information; ");
			result = pstmt->executeQuery();

			while (result->next()) { //회원가입이 되어있을 경우 else문에서 정상동작 
				find_nick = result->getString("nick_name");
				if (find_nick == input_nick) {
					end_client = 1;
					break;
				}
			}

			if (!end_client) {
				send(client_sock, "회원탈퇴로 인한 종료", strlen("회원탈퇴로 인한 종료"), 0);
				cout << "회원 탈퇴 되었습니다" << endl;
				cout << "더이상 메세지를 전송하실수 없습니다" << endl;
				exit(1);
			}
			else {
				string text;
				std::getline(cin, text);
				const char* buffer = text.c_str();
				send(client_sock, buffer, strlen(buffer), 0);
				end_client = 0;
			}
		}
		th2.join();
		closesocket(client_sock); //리소스 반환 
	}
	//반납
	WSACleanup();
	delete result;
	delete pstmt;
	delete con;
	system("pause");

	return 0;
}

void korean() {

	con->setSchema("chatprogram");
	stmt = con->createStatement();
	stmt->execute("set names euckr"); // 한글 인코딩을 위함
	if (stmt) { delete stmt; stmt = nullptr; }
}

void recv_prev_msg() {

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
}

void duplicate_login(string input, string query, bool check, string* create_input) {

	string find;

	while (check == 1) {

		check = 1;

		cout << input << "를 입력하세요 -> \n";
		cin >> *create_input;

		// ID 확인 -> check_id == 0;
		con->setSchema("chatprogram");
		pstmt = con->prepareStatement(query);
		result = pstmt->executeQuery();

		while (result->next()) {
			find = result->getString(input);
			if (find == *create_input) {
				check = 0;
			}
		}

		if (check == 1) { cout << "잘못된 " << input << "입니다." << endl << "다시 입력하세요" << endl; }
	}
}
