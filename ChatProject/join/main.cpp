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

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ� 
const string username = "project"; // �����ͺ��̽� ����� 
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ 

void create_table();
void information_insert(string nick_name, string name, string id, string pw);
void korean();
void duplicate(string input, string *creat_input, string *find_id, string query);
void drop_out_duplicate(string* id, string* find_id);

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
        //////////////////////////////���� ���� �Է��ϱ�//////////////////////////////////////
        int sign_inout = 0;
        cout << "1 : ȸ������" << endl << "2 : ȸ�� Ż��" << endl << "��ȣ�� �Է��ϼ���" << endl;
        cin >> sign_inout;

        switch (sign_inout)
        {
        case 1:////////////////////ȸ�� ����//////////////////////////////////////////////////////
            //�̸� �Է�
            cout << "�̸��� �Է����ּ��� : ";
            cin >> name;
            //���̵� �ߺ� �˻�
            duplicate("id", &id, &find_id, "SELECT id FROM information;");
            //�г��� �ߺ� �˻�
            duplicate("nick_name", &nick_name, &find_nick, "SELECT nick_name FROM information;");
            //��й�ȣ �Է� 
            cout << "��й�ȣ�� �Է����ּ��� : ";
            cin >> pw;
            //���������� nick_name, name,id,pw�� information ������ ���̽��� �߰��ϱ� 
            information_insert(nick_name, name, id, pw);
            break;

        case 2:////////////////////ȸ�� Ż��//////////////////////////////////////////////////////
            drop_out_duplicate(&id, &find_id);
            break;
        default:
            cout << "�ٽ� �Է��ϼ���" << endl;
            break;
        }
    }
    //�ݳ�
    delete stmt;
    delete pstmt;
    delete con;
    delete result;
    system("pause");
    return 0;
}
void korean() {// �ѱ� ���ڵ��� ����
    con->setSchema("chatprogram");
    stmt = con->createStatement();
    stmt->execute("set names euckr"); 
    if (stmt) { delete stmt; stmt = nullptr; }
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

    pstmt->setString(1, "Alice"); 
    pstmt->setString(2, "������"); 
    pstmt->setString(3, "lee123"); 
    pstmt->setString(4, "4321"); 
    pstmt->execute();

    pstmt->setString(1, "Bunny"); 
    pstmt->setString(2, "������"); 
    pstmt->setString(3, "song77");
    pstmt->setString(4, "6839");
    pstmt->execute();

    pstmt->setString(1, "Amy"); 
    pstmt->setString(2, "�ڿ���");
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
    pstmt->setString(1, nick_name); //�г��� (�ߺ� �Ұ�)
    pstmt->setString(2, name); //�̸�
    pstmt->setString(3, id); //id (�ߺ� �Ұ�)
    pstmt->setString(4, pw); // pw
    pstmt->execute();
    cout << "One row inserted." << endl;
}
void duplicate(string input, string *creat_input,string *find_id, string query) {
    while (1) {
        cout << "������" << input << ((input == "id") ? "��" : "��") << " �Է��ϼ��� : " << endl;
        cin >> *creat_input;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement(query);
        result = pstmt->executeQuery();

        while (result->next()) {
            *find_id = result->getString(input);
            if (find_id == creat_input) { break; }
        }
        if (find_id == creat_input) {
            cout << input <<"�� �ߺ��Ǿ����ϴ�." << endl << "�ٽ� �Է��ϼ���" << endl;
            continue;
        }
        break;
    }
}
void drop_out_duplicate(string *id, string *find_id) { ///////////ȸ�� Ż��/////////////////
    korean();
    while(1){        
        cout << "���̵� �Է����ּ��� : ";
        cin >> *id;

        con->setSchema("chatprogram");
        pstmt = con->prepareStatement("SELECT id FROM information;");
        result = pstmt->executeQuery();

        while (result->next()) {
            *find_id = result->getString("id");
            if (*find_id == *id) { break; }
            else { *find_id = "NONE"; }
        }

        if (*find_id != *id) {
            cout << "ID�� �������� �ʽ��ϴ�." << endl << "�ٽ� �Է��ϼ���" << endl;
            continue;
        }
        //drop
        pstmt = con->prepareStatement("DELETE FROM information WHERE id = ?");
        pstmt->setString(1, *id);
        result = pstmt->executeQuery();
        cout << *id << "�� ȸ�� Ż�� �Ǿ����ϴ�" << endl;
        break;
    }
}
