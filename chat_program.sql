-- cpp_db 이름의 데이터베이스 생성
CREATE DATABASE chatprogram CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;

-- user 이름의 사용자 생성
CREATE USER 'project'@'%' IDENTIFIED BY '1234';

-- user 사용자에게 모든 권한 부여
GRANT ALL PRIVILEGES ON *.* TO 'project'@'%' WITH GRANT OPTION;

-- 현재 유저 목록 조회
SELECT user FROM mysql.user;