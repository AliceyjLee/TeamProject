#include <stdlib.h> 
#include <iostream> 
#include "mysql_connection.h" 
#include <cppconn/driver.h> 
#include <cppconn/exception.h> 
#include <cppconn/prepared_statement.h> 
using namespace std;

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 
const string username = "user"; // 데이터베이스 사용자 
const string password = "1234"; // 데이터베이스 접속 비밀번호 

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;

void create_table();
void information_insert(string nick, string name, string id, string pw);
void message_insert(int index, string nick, string msg);

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
    create_table();

    string nick_name;
    string name;
    string id;
    string pw;

    //회원가입
    while (1) {
        cout << "생성할 id를 입력하세요 : " << endl;
        cin >> id;
        pstmt = con->prepareStatement("SELECT * FROM information WHERE LIKE ?;");
        pstmt->setString(1, id);
        result = pstmt->executeQuery(); //executeQuery는 결과값 담을 수 있음 
        //출력해서 확인하기 
    }



    delete pstmt;
    delete con;
    system("pause");
    return 0;
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

    con->setSchema("CHatProject");  //CHatProject로 스키마 만들기(워크벤치에서) 

    // information 테이블 생성
    stmt = con->createStatement(); //회원가입 내역 저장 시 워크벤치에서 미리 테이블 생성하기(insert기능만 구현)
    stmt->execute("DROP TABLE IF EXISTS information"); // 회원가입 내역 저장 시 지워주기
    cout << "Finished dropping table (if existed)" << endl;// 회원가입 내역 저장 시 지워주기
    stmt->execute("CREATE TABLE information (nick_name VARCHAR(20) PRIMARY KEY, name VARCHAR(20), id VARCHAR(20), pw VARCHAR(20));");
    //cout << "Finished creating table" << endl;
    delete stmt;

    // message 테이블 생성
    stmt = con->createStatement(); //회원가입 내역 저장 시 워크벤치에서 미리 테이블 생성하기(insert기능만 구현)
    stmt->execute("DROP TABLE IF EXISTS message"); // 회원가입 내역 저장 시 지워주기
    cout << "Finished dropping table (if existed)" << endl;// 회원가입 내역 저장 시 지워주기
    stmt->execute("CREATE TABLE message (index serial PRIMARY KEY, user_nick_name VARCHAR(20), message VARCHAR(100));");
    //cout << "Finished creating table" << endl;
    delete stmt;

}

void information_insert(string nick, string name, string id, string pw) {
    //information insert
    pstmt = con->prepareStatement("INSERT INTO information(nick_name, name,id,pw) VALUES(?,?,?,?)");
    pstmt->setString(1, nick); //닉네임 (중복 시 다시 입력)
    pstmt->setString(2, name); //이름
    pstmt->setString(3, id); //id (중복 시 다시 입력)
    pstmt->setString(4, pw); // pw
    pstmt->execute();
    //cout << "One row inserted." << endl;
}

void message_insert(int index, string nick, string msg) {
    //message insert
    pstmt = con->prepareStatement("INSERT INTO message(index, user_nick_name,message) VALUES(?,?,?)");
    pstmt->setInt(1, index); //idx (한번 입력할때마다 count되는 변수 생성) 
    pstmt->setString(2, nick); //닉네임
    pstmt->setString(3, msg); //메세지
    pstmt->execute();
    //cout << "One row inserted." << endl;
}