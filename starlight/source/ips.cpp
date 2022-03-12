#include "ips.hpp"

const char* smo::getServerIp(bool useLocal){
    static smo::ServerIP ips;
    if(useLocal) return ips.localIp;
    else return ips.privateIp;
}