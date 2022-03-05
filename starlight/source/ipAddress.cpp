#include "ipAddress.hpp"

smo::ServerIP& smo::getServerIp(){
    static smo::ServerIP ip;
    return ip;
}