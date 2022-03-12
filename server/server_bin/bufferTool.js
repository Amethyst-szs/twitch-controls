const fs = require("fs");
const log = require("./console");

module.exports = {
  disconnectCheck: function (msg, CurDir) {
    //Grab the disconnect template buffer from the buffer file
    const buf = Buffer.from(
      fs.readFileSync(`${CurDir}/buffers/disconnect.buf`)
    );
    //Check if this was a disconnect request
    wasDisconnet = Buffer.compare(msg, buf) == 0;
    if (wasDisconnet)
      log.log(1, "Client has requested a disconnect, clearing...");
    return Buffer.compare(msg, buf) == 0;
  },

  demoUpdate: function (msg, CurDir, invalidStage) {
    //Load in the template buffer from the buffer file
    fileBuf = Buffer.from(fs.readFileSync(`${CurDir}/buffers/demoUpdate.buf`));
    //Replace the final byte in the file with the final byte of the msg from the switch
    //This is done so you can compare them regardless if this is an on or off request
    fileBuf.fill(msg[msg.byteLength - 1], fileBuf.byteLength - 1);
    if (Buffer.compare(msg, fileBuf) == 0) {
      switch (msg[msg.byteLength - 1]) {
        case 48: //Boolean value of false
          log.log(1, "Not in demo scene, starting/resuming packet transfer");
          return false;
        case 49: //Boolean value of true
          log.log(1, "Entered Demo scene! Not sending any packets!!");
          return true;
        default:
          //Some weird bug happened, look into it if this is ever seen
          log.log(1, "Invalid bool value for Demo Toggle??");
      }
    }
    //If it ended up not being a log about the scene, just return what we started with
    return invalidStage;
  },

  reject: function (msg, CurDir) {
    //Prepare a return object
    returnVal = {};

    //Load in the template buffer from the buffer file
    fileBuf = Buffer.from(fs.readFileSync(`${CurDir}/buffers/reject.buf`));
    //Replace 3rd to last byte with Rejection ID
    //Replace last byte with rejection state

    //Rejection ID byte ranges from 31-39, subtract 30 and you get the ID 1-9
    //Rejection sate byte is a bool on 30-31, 30 being success 31 being rejection

    //Rejection ID from message into template
    fileBuf.fill(
      msg[msg.byteLength - 3],
      fileBuf.byteLength - 3,
      fileBuf.byteLength - 2
    );
    //Toggle state from message into template
    fileBuf.fill(msg[msg.byteLength - 1], fileBuf.byteLength - 1);

    if (Buffer.compare(msg, fileBuf) == 0) {
      returnVal.wasRejectLog = true;
      returnVal.rejectionID = parseInt(msg[msg.byteLength - 3], 10) - 48;
      returnVal.rejectionState = parseInt(msg[msg.byteLength - 1], 10) - 48;
      if (returnVal.rejectionState == 0) returnVal.rejectionState = true;
      else returnVal.rejectionState = false;
    } else returnVal.wasRejectLog = false;

    //Return an object
    return returnVal;
  },

  Vector3fBuf: function (SocketID, FloatArray) {
    const buf = Buffer.alloc(13);

    buf.writeInt8(SocketID, 0);
    buf.writeFloatLE(FloatArray[0], 1);
    buf.writeFloatLE(FloatArray[1], 5);
    buf.writeFloatLE(FloatArray[2], 9);

    return buf;
  },

  GenericBuf: function (SocketID, EventID) {
    const buf = Buffer.alloc(2);
    buf.writeInt8(SocketID, 0);
    buf.writeInt8(EventID, 1);
    return buf;
  },
};
