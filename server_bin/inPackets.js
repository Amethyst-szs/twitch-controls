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
        log.log(0, `Log: ${msg.toString().substring(1)}`);
    }
}