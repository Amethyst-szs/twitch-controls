const RedeemSet = require('../settings/redeem_set.json');
const Rand = require('./advancedRand');
const bufferTool = require('./bufferTool');

module.exports = {
    //Channel point redeem: PrevScene (Server -> Client)
    Events: function(server, EventID, client){ //Socket ID 1 - Event ID 1
        server.send(bufferTool.GenericBuf(1, EventID), client.port, client.address);
    },

    //Channel point redeem: Resize (Server -> Client)
    Resize: function(server, client){ //Socket ID 2
        const Amplitude = RedeemSet.Resize.Amplitude;
        const Scale = [Rand.Ampli(Amplitude, false), Rand.Ampli(Amplitude, false), Rand.Ampli(Amplitude, false)];
        server.send(bufferTool.Vector3fBuf(2, Scale), client.port, client.address);
    },

    //Channel point redeem: PosRandomize (Server -> Client)
    PosRandomize: function(server, client){ //Socket ID 3
        const Amplitude = RedeemSet.PosRandomize.Amplitude;
        const NewPos = [Rand.Ampli(Amplitude, true), Rand.Ampli(Amplitude, false), Rand.Ampli(Amplitude, true)];
        server.send(bufferTool.Vector3fBuf(3, NewPos), client.port, client.address);
    } 
}