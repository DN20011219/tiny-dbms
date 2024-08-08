// Copyright (c) 2024 by dingning
//
// file  : session.h
// since : 2024-08-07
// desc  : this session will store information about the connection between client and server. Include follow information:
// 1、user identity: root/user
// 2、connect dbms: all user operation can only change the data belongs the connect dbms; 
// root user can change each dbms except the default dbms, which will store all information about this system
// 3、sql execute state: one user can only submit one sql to execute at one time, so client can watch this state to control the client opertion.

#ifndef VDBMS_BENCH_SESSION_H_
#define VDBMS_BENCH_SESSION_H_

namespace tiny_v_dbms {

enum UserIdentity
{
    ROOT,
    USER
};

class Connection
{
    
    
};

}

#endif // VDBMS_BENCH_SESSION_H_