const bufferTool = require('./bufferTool');
const log = require('./console');

module.exports={
    //Initalize of client test (Client -> Server)
    DummyInit: function(msg, rinfo){ //Packet ID -1
        log.log(0, `DummyInit - ${rinfo.address}:${rinfo.port}`);
    },

    //Actual initalization of client (Client -> Server)
    Init: function(msg, rinfo){ //Packet ID -2
        log.log(0, `Init - ${rinfo.address}:${rinfo.port}`);
        return rinfo;
    },

    //Log any bit of information (Client -> Server)
    Log: function(msg, rinfo, CurDir){ //Packet ID -3
        if(Buffer.compare(msg, bufferTool.disconnectBuffer(CurDir)) == 0){
            return true;
        }
        log.log(0, `Log: ${msg.toString().substring(1)}`);
        return false;
    },

    demoToggle(msg, rinfo){ //Packet ID -4
        log.log(0, `Invalid stage rejection status set to: ${msg.toString().substring(0)}`);
        if(parseInt(msg.toString().substring(1), 10) == 1) return true;
        else return false;
    }
}