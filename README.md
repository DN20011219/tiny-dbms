# tiny-dbms
A tiny dbms, includes some basic knowledge about dbms, and some implements of them.

# 1. how to start
## 1.1 Environment
### 1.1.1 cmake
```
website [https://cmake.org/download/]

cmake version & support c++ version [https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html]

cmake need to support c++ 17, so cmake version must newer than 3.8.
```

### 1.1.2 clang
```
clang version & support c++ version [https://clang.llvm.org/cxx_status.html]

clang must newer than 5.
```
## 1.2 run server
```
mkdir build
cd build
cmake ..
make
./app
```
## 1.3 run client
```
cd include/client
clang++ -std=c++17 client.cpp -o client 
```
### 1.3.1 connect to server
input connection information on client:
```
root  (connect base db, can create new database)
```
```
user  (connect to user database, can execute other all sql)
```
![image](https://github.com/user-attachments/assets/91296a86-6baf-44bd-a2f9-02e26d0bbb86)

### 1.3.2 execute sql
Input sql in client, then you can get the excution result.
Example:

![image](https://github.com/user-attachments/assets/3b9aaa76-40c4-4e49-a849-1bfe0cac8d51)

# 2. sql supported
## 2.1 base version
![image](https://github.com/user-attachments/assets/e4c5b122-cb99-46a9-a675-31d3d93d6d86)
## 2.2 olap dbms version
## 2.3 graph dbms version
## 2.4 vector dbms version

# 3. index support
## 3.1 base version
None index supported now.
## 3.2 olap dbms version
## 3.3 graph dbms version
## 3.4 vector dbms version