#pragma comment(lib, "ws2_32.lib")
#include <iostream>
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
void korean();
void duplicate(string input, string *creat_input, string find_id, string query);
void drop_out_duplicate(string* id, string find_id);

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
    korean();
    create_table(); 
    string nick_name,name,id,pw;
    string find_id, find_nick;

    while (1){
        //////////////////////////////다음 동작 입력하기//////////////////////////////////////
        int sign_inout = 0;
        cout << "1 : 회원가입" << endl << "2 : 회원 탈퇴" << endl << "번호를 입력하세요" << endl;
        cin >> sign_inout;

        switch (sign_inout)
        {
        case 1:////////////////////회원 가입//////////////////////////////////////////////////////
            //이름 입력
            cout << "이름을 입력해주세요 : ";
            cin >> name;
            //아이디 중복 검사
            duplicate("id", &id, find_id, "SELECT id FROM information;");
            //닉네임 중복 검사
            duplicate("nick_name", &nick_name, find_nick, "SELECT nick_name FROM information;");
            //비밀번호 입력 
            cout << "비밀번호를 입력해주세요 : ";
            cin >> pw;
            //최종적으로 nick_name, name,id,pw를 information 데이터 베이스에 추가하기 
            information_insert(nick_name, name, id, pw);
            break;

        case 2:////////////////////회원 탈퇴//////////////////////////////////////////////////////
            drop_out_duplicate(&id, find_id);
            break;
        default:
            cout << "다시 입력하세요" << endl;
            break;
        }
    }
    //반납
    delete stmt;
    delete pstmt;
    delete con;
    delete result;
    system("pause");
    return 0;
}
void korean() {// 한글 인코딩을 위함
    con->setSchema("chatprogram");
    stmt = con->createStatement();
    stmt->execute("set names euckr"); 
    if (stmt) { delete stmt; stmt = nullptr; }
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

    pstmt->setString(1, "Alice"); 
    pstmt->setString(2, "이윤정"); 
    pstmt->setString(3, "lee123"); 
    pstmt->setString(4, "4321"); 
    pstmt->execute();

    pstmt->setString(1, "Bunny"); 
    pstmt->setString(2, "송혜교"); 
    pstmt->setString(3, "song77");
    pstmt->setString(4, "6839");
    pstmt->execute();

    pstmt->setString(1, "Amy"); 
    pstmt->setString(2, "박연진");
    pstmt->setString(3, "park44"); 
    pstmt->setString(4, "4444"); 
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
    korean();

  //information insert
    pstmt = con->prepareStatement("INSERT INTO information(nick_name,name,id,pw) VALUES(?,?,?,?)");
    pstmt->setString(1, nick_name); //닉네임 (중복 불가)
    pstmt->setString(2, name); //이름
    pstmt->setString(3, id); //id (중복 불가)
    pstmt->setString(4, pw); // pw
    pstmt->execute();
    cout << "One row inserted." << endl;
}
void duplicate(string input, string *creat_input,string find_id, string query) {
    while (1) {
        cout << "생성할" << input << ((input == "id") ? "를" : "을") << " 입력하세요 : " << endl;
        cin >> *creat_input;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement(query);
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString(input);
            if (find_id == *creat_input) { break; }
        }
        if (find_id == *creat_input) {
            cout << input <<"가 중복되었습니다." << endl << "다시 입력하세요" << endl;
            continue;
        }
        break;
    }
}
void drop_out_duplicate(string *id, string find_id) { ///////////회원 탈퇴/////////////////
    korean();
    while(1){        
        cout << "아이디를 입력해주세요 : ";
        cin >> *id;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement("SELECT id FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString("id");
            if (find_id == *id) { break; }
            else { find_id = "NONE"; }
        }

        if (find_id != *id) {
            cout << "ID가 존재하지 않습니다." << endl << "다시 입력하세요" << endl;
            continue;
        }
        //drop
        pstmt = con->prepareStatement("DELETE FROM information WHERE id = ?");
        pstmt->setString(1, *id);
        result = pstmt->executeQuery();
        cout << *id << "님 회원 탈퇴 되었습니다" << endl;
        break;
    }
}
