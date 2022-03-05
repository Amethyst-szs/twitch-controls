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
    Log = (u8)-3
};

class OutPacket {
public:
    virtual u32 calcLen() { return 0; }
    virtual void construct(u8* dst) { }
};

enum InPacketType : u8 {
    Event = (u8)1,
    Resize = (u8)2,
    PosRandomize = (u8)3
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
        Warning = 1,
        Error = 2,
        Fatal = 3
    };
    LogType type;
    char* message;
    u32 calcLen();
    void construct(u8* dst);
};

class InPacketEvent : public InPacket{
u8 eventID;
public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketResize : public InPacket{
sead::Vector3f scaleVector;
public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};

class InPacketPosRandomize : public InPacket{
sead::Vector3f posVector;
public:
    void parse(const u8* data, u32 len);
    void on(Server& server);
};
} //  namespace smo