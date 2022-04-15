#pragma once

#include <fl/server.h>
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>
#include <sead/prim/seadSafeString.h>
#include <types.h>

namespace smo {
class Server;
// negative numbers for outgoing, positive for incoming
enum OutPacketType : u8 {
    DummyInit = (u8)-1,
    Init = (u8)-2,
    Log = (u8)-3,
    Disconnect = (u8)-4,
    Pong = (u8)-5,
    Restrict = (u8)-6,
    DemoToggle = (u8)-7,
    Reject = (u8)-8
};

class OutPacket {
public:
    virtual u32 calcLen() { return 0; }
    virtual void construct(u8* dst) { }
};

enum InPacketType : u8 {
    Event = (u8)1,
    Resize = (u8)2,
    PosRandomize = (u8)3,
    Ping = (u8)4,
    Say = (u8)5,
    Kick = (u8)6
};

class InPacket {
public:
    virtual void parse(const u8* data, u32 len) { }
    virtual void on(Server& server) { }
};

class OutPacketLog : public OutPacket {
public:
    enum LogType : u8 {
        Log = 0,
        DemoToggle = 1,
        Error = 2,
        Fatal = 3
    };
    LogType type;
    char* message;
    u32 calcLen();
    void construct(u8* dst);
};

// class OutPacketDemoToggle : public OutPacket {
// public:
//     bool toggleState;
//     u32 calcLen();
//     void construct(u8* dst);
// };

class InPacketEvent : public InPacket {
    u8 eventID;
    bool isTwitch;

public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketPing : public InPacket {
public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketKick : public InPacket {
public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketResize : public InPacket {
    sead::Vector3f scaleVector;

public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketPosRandomize : public InPacket {
    sead::Vector3f posVector;

public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketSay : public InPacket {
    const char* message;
    int textFrames = 0;

public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};
} //  namespace smo