# 01\_main\_minimal.md — 실행 방법

## 1. 빌드

```bash
> cl sqlite-study\code\main_minimal.c sqlite3.c
```

* `cl` : MSVC 컴파일러
* `main_minimal.c` : 최소 예제
* `sqlite3.c` : SQLite 전체 구현 파일

---

## 2. 실행

```bash
> main_minimal.exe test.db
```

* `test.db` : 생성/연결할 SQLite DB 파일
* 파일이 없으면 새로 생성됨

---

## 3. 예시

```bash
Opened DB: test.db
Enter SQL (end with ';', type 'exit;' to quit)
sqlite> CREATE TABLE IF NOT EXISTS test(id INTEGER, name TEXT);
sqlite> INSERT INTO test VALUES(1, 'Alice');
(1 row affected)
sqlite> INSERT INTO test VALUES(2, 'Bob');
(1 row affected)
sqlite> SELECT * FROM test;
1 | Alice
2 | Bob
1 | Alice
2 | Bob
(1 row affected)
sqlite> exit
SQL error: near "exit": syntax error
sqlite> exit;
```
