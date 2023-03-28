-- cpp_db 이름의 데이터베이스 생성
CREATE DATABASE chatprogram CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;

-- user 이름의 사용자 생성
CREATE USER 'project'@'%' IDENTIFIED BY '1234';

-- user 사용자에게 모든 권한 부여
GRANT ALL PRIVILEGES ON *.* TO 'project'@'%' WITH GRANT OPTION;

-- 현재 유저 목록 조회
SELECT user FROM mysql.user;

DROP TABLE IF EXISTS information;
CREATE TABLE information(
   nick_name VARCHAR(50) NOT NULL PRIMARY KEY, -- PRIMARY KEY는 NOT NULL이 포함되어 있어서 안써도된다  
    name VARCHAR(50) NOT NULL,
    id VARCHAR(50) NOT NULL,
    pw VARCHAR(50) NOT NULL
 );
 
 DROP TABLE IF EXISTS message;
 CREATE TABLE message(
   user_index VARCHAR(50) NOT NULL PRIMARY KEY, -- PRIMARY KEY는 NOT NULL이 포함되어 있어서 안써도된다  
    user_nick_name VARCHAR(50) NOT NULL,
    user_message VARCHAR(50) NOT NULL,
    FOREIGN KEY(user_nick_name) REFERENCES information(nick_name) ON UPDATE CASCADE ON DELETE CASCADE
 );
 
 INSERT INTO information VALUES 
   ('Adam ', '김민지', 'kim0000', '1234'),
   ('Alice ', '이윤정', 'lee123', '4321'),
    ('Bunny  ', '송혜교', 'song77', '6839'),
    ('Amy ', '박연진', 'park44', '4444');
    
SHOW TABLES;
SELECT * FROM information;
