# tiny-vector-dbms
A tiny vector dbms, includes some basic knowledge about vdbms, and some implements of them.

# 1. how to start
## use terminal:
### run server
```
mkdir build
cd build
cmake ..
make
./app
```
### run client
```
cd include/client
clang++ -std=c++17 client.cpp -o client 
```
### handle connection
input connection information on client:
```
root  (connect base db, can create new database)
```
```
user  (connect to user database, can execute other all sql)
```
![image](https://github.com/user-attachments/assets/91296a86-6baf-44bd-a2f9-02e26d0bbb86)

### execute sql
example:
![image](https://github.com/user-attachments/assets/3b9aaa76-40c4-4e49-a849-1bfe0cac8d51)

# 2. sql support
## base version
![image](https://github.com/user-attachments/assets/e4c5b122-cb99-46a9-a675-31d3d93d6d86)
## graph dbms version
## vector dbms version
