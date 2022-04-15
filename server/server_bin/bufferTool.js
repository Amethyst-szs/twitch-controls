module.exports = {
  Vector3fBuf: function (SocketID, FloatArray) {
    const buf = Buffer.alloc(13);

    buf.writeInt8(SocketID, 0);
    buf.writeFloatLE(FloatArray[0], 1);
    buf.writeFloatLE(FloatArray[1], 5);
    buf.writeFloatLE(FloatArray[2], 9);
    return buf;
  },

  sayBuf: function(msg){
    //Construct buffer
    const buf = Buffer.alloc(msg.length+2); //One extra byte for socket ID, one extra for null terminator
    buf.fill(0, 1, buf.length);

    //Write msg into buffer with filler byte at start
    msg = '.'+msg;
    buf.write(msg, 'utf-8');

    //Write Socket ID into byte 0
    buf.writeInt8(5, 0);
    buf.writeInt8(0x0, buf.length-1);

    // console.log(buf);
    return buf;
  },

  GenericBuf: function (SocketID, EventID, isTwitch) {
    const buf = Buffer.alloc(3);
    buf.writeInt8(SocketID, 0);
    buf.writeInt8(EventID, 1);
    buf.writeUInt8(isTwitch, 2);
    return buf;
  },
};
