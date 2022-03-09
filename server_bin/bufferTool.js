const fs = require('fs');
const log = require('./console');

module.exports = {
    disconnectCheck: function(msg, CurDir){
        //Grab the disconnect template buffer from the buffer file
        const buf = Buffer.from(fs.readFileSync(`${CurDir}/settings/disconnect.buf`));
        //Check if this was a disconnect request
        wasDisconnet = Buffer.compare(msg, buf) == 0;
        if(wasDisconnet) log.log(1, "Client has requested a disconnect, clearing...");
        return Buffer.compare(msg, buf) == 0;
    },

    demoUpdate: function(msg, CurDir, invalidStage){
        //Load in the template buffer from the buffer file
        fileBuf = Buffer.from(fs.readFileSync(`${CurDir}/settings/demoUpdate.buf`));
        //Replace the final byte in the file with the final byte of the msg from the switch
        //This is done so you can compare them regardless if this is an on or off request
        fileBuf.fill(msg[msg.byteLength-1], fileBuf.byteLength-1);
        if(Buffer.compare(msg, fileBuf) == 0){
            switch(msg[msg.byteLength-1]){
                case 48: //Boolean value of false
                    log.log(1, "Not in demo scene, starting/resuming packet transfer");
                    return false;
                case 49: //Boolean value of true
                    log.log(1, "Entered Demo scene! Not sending any packets!!");
                    return true;
                default: //Some weird bug happened, look into it if this is ever seen
                    log.log(1, "Invalid bool value for Demo Toggle??");
            }
        }
        //If it ended up not being a log about the scene, just return what we started with
        return invalidStage;
    },

    Vector3fBuf: function(SocketID, FloatArray){
        const buf = Buffer.alloc(13);

        buf.writeInt8(SocketID, 0);
        buf.writeFloatLE(FloatArray[0], 1);
        buf.writeFloatLE(FloatArray[1], 5);
        buf.writeFloatLE(FloatArray[2], 9);

        return buf;
    },

    GenericBuf: function(SocketID, EventID){
        const buf = Buffer.alloc(2);
        buf.writeInt8(SocketID, 0);
        buf.writeInt8(EventID, 1);
        return buf;
    }
}