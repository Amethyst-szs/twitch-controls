const fs = require('fs');

module.exports = {
    disconnectBuffer: function(CurDir){
        const buf = Buffer.from(fs.readFileSync(`${CurDir}/settings/disconnect.buf`));
        return buf;
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