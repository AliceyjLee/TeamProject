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

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ� 
const string username = "project"; // �����ͺ��̽� ����� 
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ 

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
    stmt->execute("set names euckr"); // �ѱ� ���ڵ��� ����
    if (stmt) { delete stmt; stmt = nullptr; }

    create_table(); 
    string nick_name,name,id,pw;
    string find_id, find_nick;

    /////////////////////////////////////////////////ȸ������///////////////////////////////////////////////////////

    //�̸� �Է�
    cout << "�̸��� �Է����ּ��� : ";
    cin >> name;

    //���̵� �ߺ� �˻�
    while (1){
        cout << "������ id�� �Է��ϼ��� : " << endl;
        cin >> id;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement("SELECT id FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString("id");
            if (find_id == id) { break; }
        }
        if (find_id == id) {
            cout << "ID�� �ߺ��Ǿ����ϴ�." << endl << "�ٽ� �Է��ϼ���" << endl;
            continue;
        }
        break;
    }

    //�г��� �ߺ� �˻�
    while (1) {
        cout << "������ �г����� �Է��ϼ��� : " << endl;
        cin >> nick_name;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement("SELECT nick_name FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString("nick_name");
            if (find_id == nick_name) { break; }
        }
        if (find_id == nick_name) {
            cout << "�г����� �ߺ��Ǿ����ϴ�." << endl << "�ٽ� �Է��ϼ���" << endl;
            continue;
        }
        break;
    }
    //��й�ȣ �Է� 
    cout << "��й�ȣ�� �Է����ּ��� : ";
    cin >> pw;

    cout << "���!" << endl;
    cout << nick_name << endl;
    cout << name << endl;
    cout << id << endl;
    cout << pw << endl;

    //���������� nick_name, name,id,pw�� information ������ ���̽��� �߰��ϱ� 
    information_insert(nick_name, name, id, pw);
    
    //�ݳ�
    delete stmt;
    delete pstmt;
    delete con;
    delete result;
    system("pause");
    return 0;
}

void create_table() {

    con->setSchema("chatprogram");
    // information ���̺� ����
    stmt = con->createStatement(); 
    stmt->execute("DROP TABLE IF EXISTS information");
    cout << "Finished dropping table (if existed)" << endl;
    stmt->execute("CREATE TABLE information (nick_name VARCHAR(20) PRIMARY KEY NOT NULL, name VARCHAR(20) NOT NULL, id VARCHAR(20) NOT NULL, pw VARCHAR(20) NOT NULL);");
    cout << "Finished creating table" << endl;
       
    ///////////information Table ����Ʈ�� �߰�////////////
    pstmt = con->prepareStatement("INSERT INTO information (nick_name, name, id, pw) VALUES(?,?,?,?)");
    pstmt->setString(1,"Adam"); //�г��� (�ߺ� �Ұ�)
    pstmt->setString(2,"�����"); //�̸�
    pstmt->setString(3,"kim0000"); //id (�ߺ� �Ұ�)
    pstmt->setString(4,"1234"); // pw
    pstmt->execute();

    pstmt->setString(1, "Alice"); //�г��� (�ߺ� �Ұ�)
    pstmt->setString(2, "������"); //�̸�
    pstmt->setString(3, "lee123"); //id (�ߺ� �Ұ�)
    pstmt->setString(4, "4321"); // pw
    pstmt->execute();

    pstmt->setString(1, "Bunny"); //�г��� (�ߺ� �Ұ�)
    pstmt->setString(2, "������"); //�̸�
    pstmt->setString(3, "song77"); //id (�ߺ� �Ұ�)
    pstmt->setString(4, "6839"); // pw
    pstmt->execute();

    pstmt->setString(1, "Amy"); //�г��� (�ߺ� �Ұ�)
    pstmt->setString(2, "�ڿ���"); //�̸�
    pstmt->setString(3, "park44"); //id (�ߺ� �Ұ�)
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
    stmt->execute("set names euckr"); // �ѱ� ���ڵ��� ����
    if (stmt) { delete stmt; stmt = nullptr; }

  //information insert
    pstmt = con->prepareStatement("INSERT INTO information(nick_name,name,id,pw) VALUES(?,?,?,?)");
    pstmt->setString(1, nick_name); //�г��� (�ߺ� �Ұ�)
    pstmt->setString(2, name); //�̸�
    pstmt->setString(3, id); //id (�ߺ� �Ұ�)
    pstmt->setString(4, pw); // pw
    pstmt->execute();
    cout << "One row inserted." << endl;
}