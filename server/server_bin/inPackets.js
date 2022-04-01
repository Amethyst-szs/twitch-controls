const bufferTool = require("./bufferTool");
const log = require("./console");

module.exports = {
  //Initalize of client test (Client -> Server)
  // DummyInit: function (msg, rinfo) {
  //   //Packet ID -1
  //   log.log(0, `Dummy Init request from a client`);
  // },

  //Actual initalization of client (Client -> Server)
  // Init: function (msg, rinfo) {
  //   //Packet ID -2
  //   //Return
  //   log.log(0, `Initializing a client!`);
  //   return rinfo;
  // },

  //Log any bit of information (Client -> Server)
  Log: function (msg, rinfo, CurDir) {
    //Packet ID -3
    log.log(0, `${msg.toString().substring(1)}`);
  },
};
