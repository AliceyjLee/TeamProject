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
#include <cppconn/statement.h>

#define MAX_SIZE 1024 
#define MAX_CLIENT 3
using std::cout;
using std::cin;
using std::endl;
using std::string;

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 
const string username = "project"; // 데이터베이스 사용자 
const string password = "1234"; // 데이터베이스 접속 비밀번호 

void create_table();
void information_insert(string nick_name, string name, string id, string pw);

int main()
{
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
    stmt = con->createStatement();
    stmt->execute("set names euckr"); // 한글 인코딩을 위함
    if (stmt) { delete stmt; stmt = nullptr; }

    create_table(); 
    string nick_name,name,id,pw;
    string find_id, find_nick;

    /////////////////////////////////////////////////회원가입///////////////////////////////////////////////////////

    //이름 입력
    cout << "이름을 입력해주세요 : ";
    cin >> name;

    //아이디 중복 검사
    while (1){
        cout << "생성할 id를 입력하세요 : " << endl;
        cin >> id;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement("SELECT id FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString("id");
            if (find_id == id) { break; }
        }
        if (find_id == id) {
            cout << "ID가 중복되었습니다." << endl << "다시 입력하세요" << endl;
            continue;
        }
        break;
    }

    //닉네임 중복 검사
    while (1) {
        cout << "생성할 닉네임을 입력하세요 : " << endl;
        cin >> nick_name;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement("SELECT nick_name FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString("nick_name");
            if (find_id == nick_name) { break; }
        }
        if (find_id == nick_name) {
            cout << "닉네임이 중복되었습니다." << endl << "다시 입력하세요" << endl;
            continue;
        }
        break;
    }
    //비밀번호 입력 
    cout << "비밀번호를 입력해주세요 : ";
    cin >> pw;

    cout << "출력!" << endl;
    cout << nick_name << endl;
    cout << name << endl;
    cout << id << endl;
    cout << pw << endl;

    //최종적으로 nick_name, name,id,pw를 information 데이터 베이스에 추가하기 
    information_insert(nick_name, name, id, pw);
    
    //반납
    delete stmt;
    delete pstmt;
    delete con;
    delete result;
    system("pause");
    return 0;
}

void create_table() {

    con->setSchema("chatprogram");
    // information 테이블 생성
    stmt = con->createStatement(); 
    stmt->execute("DROP TABLE IF EXISTS information");
    cout << "Finished dropping table (if existed)" << endl;
    stmt->execute("CREATE TABLE information (nick_name VARCHAR(20) PRIMARY KEY NOT NULL, name VARCHAR(20) NOT NULL, id VARCHAR(20) NOT NULL, pw VARCHAR(20) NOT NULL);");
    cout << "Finished creating table" << endl;
       
    ///////////information Table 디폴트값 추가////////////
    pstmt = con->prepareStatement("INSERT INTO information (nick_name, name, id, pw) VALUES(?,?,?,?)");
    pstmt->setString(1,"Adam"); //닉네임 (중복 불가)
    pstmt->setString(2,"김민지"); //이름
    pstmt->setString(3,"kim0000"); //id (중복 불가)
    pstmt->setString(4,"1234"); // pw
    pstmt->execute();

    pstmt->setString(1, "Alice"); //닉네임 (중복 불가)
    pstmt->setString(2, "이윤정"); //이름
    pstmt->setString(3, "lee123"); //id (중복 불가)
    pstmt->setString(4, "4321"); // pw
    pstmt->execute();

    pstmt->setString(1, "Bunny"); //닉네임 (중복 불가)
    pstmt->setString(2, "송혜교"); //이름
    pstmt->setString(3, "song77"); //id (중복 불가)
    pstmt->setString(4, "6839"); // pw
    pstmt->execute();

    pstmt->setString(1, "Amy"); //닉네임 (중복 불가)
    pstmt->setString(2, "박연진"); //이름
    pstmt->setString(3, "park44"); //id (중복 불가)
    pstmt->setString(4, "4444"); // pw
    pstmt->execute();
}
void information_insert(string nick_name, string name, string id, string pw){
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
    stmt = con->createStatement();
    stmt->execute("set names euckr"); // 한글 인코딩을 위함
    if (stmt) { delete stmt; stmt = nullptr; }

  //information insert
    pstmt = con->prepareStatement("INSERT INTO information(nick_name,name,id,pw) VALUES(?,?,?,?)");
    pstmt->setString(1, nick_name); //닉네임 (중복 불가)
    pstmt->setString(2, name); //이름
    pstmt->setString(3, id); //id (중복 불가)
    pstmt->setString(4, pw); // pw
    pstmt->execute();
    cout << "One row inserted." << endl;
}