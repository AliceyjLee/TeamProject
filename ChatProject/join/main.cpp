#include <stdlib.h> 
#include <iostream> 
#include "mysql_connection.h" 
#include <cppconn/driver.h> 
#include <cppconn/exception.h> 
#include <cppconn/prepared_statement.h> 
using std::cout;
using std::cin;
using std::endl;
using std::string;

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ� 
const string username = "user"; // �����ͺ��̽� ����� 
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ 

sql::Driver* driver; 
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;

void create_table();
void information_insert(string nick, string name, string id, string pw);

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
    string find_id, find_pw;
    bool temp = 0;
    //ȸ������////////////////////////////////////////////////////////////////////////////////////////////////




    //�̸� �Է�
    cout << "�̸��� �Է� : ";
    cin >> name;


    //���̵� �ߺ� �˻�
    while (temp == 0){
        cout << "������ id�� �Է��ϼ��� : " << endl;
        cin >> id;

        con->setSchema("chatprogram");

        //select  
        pstmt = con->prepareStatement("SELECT id FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            find_id = result->getString("id");
            if (find_id == id) {
               //find_id = result->getString("id");
                //cout << find_id << endl;
                temp = 1;
                break;
            }
        }

        if (temp) {
            cout << "�ٽ� �Է��ϼ���" << endl;
            continue;
        }
    }

    cout << "�߰�!" << endl;

/*
    //�г��� �ߺ� �˻�
    while (1) {
        cout << "������ id�� �Է��ϼ��� : " << endl;
        cin >> id;

        con->setSchema("chatprogram");

        //select  
        pstmt = con->prepareStatement("SELECT id FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {

            find_id = result->getString("id");
            cout << find_id << endl;
        }
    }
   


    //��й�ȣ �Է� 
    /*
    pstmt = con->prepareStatement("SELECT * FROM information WHERE LIKE ?;");
    pstmt->setString(1, id);
    result = pstmt->executeQuery(); //executeQuery�� ����� ���� �� ����
    //����ؼ� Ȯ���ϱ�
    */


    //���������� nick_name, name,id,pw�� information������ ���̽��� �߰��ϱ� 



    //�ݳ�
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

    con->setSchema("chatprogram");  //CHatProject�� ��Ű�� �����(��ũ��ġ����) 

    // information ���̺� ����
    stmt = con->createStatement(); //ȸ������ ���� ���� �� ��ũ��ġ���� �̸� ���̺� �����ϱ�(insert��ɸ� ����)
    stmt->execute("DROP TABLE IF EXISTS information"); // ȸ������ ���� ���� �� �����ֱ�
    cout << "Finished dropping table (if existed)" << endl;// ȸ������ ���� ���� �� �����ֱ�
    stmt->execute("CREATE TABLE information (nick_name VARCHAR(20) PRIMARY KEY, name VARCHAR(20), id VARCHAR(20), pw VARCHAR(20));");
    //cout << "Finished creating table" << endl;
    delete stmt;

}

void information_insert(string nick, string name, string id, string pw) {
    //information insert
    pstmt = con->prepareStatement("INSERT INTO information(nick_name, name,id,pw) VALUES(?,?,?,?)");
    pstmt->setString(1, string(nick)); //�г��� (�ߺ� �� �ٽ� �Է�)
    pstmt->setString(2, string(name)); //�̸�
    pstmt->setString(3, string(id)); //id (�ߺ� �� �ٽ� �Է�)
    pstmt->setString(4, string(pw)); // pw
    pstmt->execute();
    //cout << "One row inserted." << endl;
}






/*

#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "user"; // �����ͺ��̽� �����
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ

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

    //ȸ������
    while (1) {
        cout << "������ id�� �Է��ϼ��� : " << endl;
        cin >> id;
        pstmt = con->prepareStatement("SELECT * FROM information WHERE LIKE ?;");
        pstmt->setString(1, id);
        result = pstmt->executeQuery(); //executeQuery�� ����� ���� �� ����
        //����ؼ� Ȯ���ϱ�
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

    con->setSchema("CHatProject");  //CHatProject�� ��Ű�� �����(��ũ��ġ����)

    // information ���̺� ����
    stmt = con->createStatement(); //ȸ������ ���� ���� �� ��ũ��ġ���� �̸� ���̺� �����ϱ�(insert��ɸ� ����)
    stmt->execute("DROP TABLE IF EXISTS information"); // ȸ������ ���� ���� �� �����ֱ�
    cout << "Finished dropping table (if existed)" << endl;// ȸ������ ���� ���� �� �����ֱ�
    stmt->execute("CREATE TABLE information (nick_name VARCHAR(20) PRIMARY KEY, name VARCHAR(20), id VARCHAR(20), pw VARCHAR(20));");
    //cout << "Finished creating table" << endl;
    delete stmt;

    // message ���̺� ����
    stmt = con->createStatement(); //ȸ������ ���� ���� �� ��ũ��ġ���� �̸� ���̺� �����ϱ�(insert��ɸ� ����)
    stmt->execute("DROP TABLE IF EXISTS message"); // ȸ������ ���� ���� �� �����ֱ�
    cout << "Finished dropping table (if existed)" << endl;// ȸ������ ���� ���� �� �����ֱ�
    stmt->execute("CREATE TABLE message (index serial PRIMARY KEY, user_nick_name VARCHAR(20), message VARCHAR(100));");
    //cout << "Finished creating table" << endl;
    delete stmt;

}

void information_insert(string nick, string name, string id, string pw) {
    //information insert
    pstmt = con->prepareStatement("INSERT INTO information(nick_name, name,id,pw) VALUES(?,?,?,?)");
    pstmt->setString(1, nick); //�г��� (�ߺ� �� �ٽ� �Է�)
    pstmt->setString(2, name); //�̸�
    pstmt->setString(3, id); //id (�ߺ� �� �ٽ� �Է�)
    pstmt->setString(4, pw); // pw
    pstmt->execute();
    //cout << "One row inserted." << endl;
}

void message_insert(int index, string nick, string msg) {
    //message insert
    pstmt = con->prepareStatement("INSERT INTO message(index, user_nick_name,message) VALUES(?,?,?)");
    pstmt->setInt(1, index); //idx (�ѹ� �Է��Ҷ����� count�Ǵ� ���� ����)
    pstmt->setString(2, nick); //�г���
    pstmt->setString(3, msg); //�޼���
    pstmt->execute();
    //cout << "One row inserted." << endl;
}


*/