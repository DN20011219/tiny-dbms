# tiny-dbms
A tiny dbms, includes some basic knowledge about dbms, and some implements of them.

# 1. how to start
use terminal
### 1.1 run server
```
mkdir build
cd build
cmake ..
make
./app
```
### 1.2 run client
```
cd include/client
clang++ -std=c++17 client.cpp -o client 
```
#### 1.2.1 connect to server
input connection information on client:
```
root  (connect base db, can create new database)
```
```
user  (connect to user database, can execute other all sql)
```
![image](https://github.com/user-attachments/assets/91296a86-6baf-44bd-a2f9-02e26d0bbb86)

#### 1.2.2 execute sql
example:

![image](https://github.com/user-attachments/assets/3b9aaa76-40c4-4e49-a849-1bfe0cac8d51)

# 2. sql support
## 2.1 base version
![image](https://github.com/user-attachments/assets/e4c5b122-cb99-46a9-a675-31d3d93d6d86)
## 2.2 olap dbms version
## 2.3 graph dbms version
## 2.4 vector dbms version

# 3. index support
## 3.1 base version
None index support now.
## 3.2 olap dbms version
## 3.3 graph dbms version
## 3.4 vector dbms version