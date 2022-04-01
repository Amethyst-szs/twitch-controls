#pragma once

#include "nn/account.h"
#include "nn/os.h"
#include <fl/packet.h>
#include <nn/socket.h>
#include <types.h>

#define SERVER_PORT 7902
#define CLIENT_PORT 7901

namespace smo {
class OutPacket;
enum OutPacketType : u8;
class InitPacket {
    u8 packetID = -2;
    nn::account::Nickname username;

public:
    void setUsername(nn::account::Nickname name);
};
class Server {
private:
    bool connected = false;
    bool started = false;

    void* threadStack = nullptr;
    nn::os::ThreadType* thread = nullptr;

public:
    void sendInit(const char* ip);
    void start();
    void connect(const char* ip);
    void disconnect();
    void sendPacket(OutPacket& packet, OutPacketType type);
    bool isConnected();
    void handlePacket(u8* buf, size_t bufSize);
    static Server& instance()
    {
        static Server s;
        return s;
    }
    s32 socket = -1;
    sockaddr server = { 0 };
    nn::account::Uid mUserID;
};
}