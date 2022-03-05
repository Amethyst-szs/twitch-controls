//Please place the local ip address of your server here!

namespace smo{
    class ServerIP{
        public:
            const char* serverIp = "192.168.0.58"; //LOCAL IP OF SERVER HERE!
    };
    ServerIP& getServerIp();
}; //namespace smo