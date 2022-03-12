const RedeemSet = require("../settings/redeem_set.json");
const Rand = require("./advancedRand");
const bufferTool = require("./bufferTool");
const log = require("./console");

//Channel point redeem: PrevScene (Server -> Client)
function Events(server, EventID, client) {
  //Socket ID 1 - Event ID 1
  server.send(bufferTool.GenericBuf(1, EventID), client.port, client.address);
}

//Channel point redeem: Resize (Server -> Client)
function Resize(server, client) {
  //Socket ID 2
  const Amplitude = RedeemSet.Resize.Amplitude;
  const Scale = [
    Rand.Ampli(Amplitude, false),
    Rand.Ampli(Amplitude, false),
    Rand.Ampli(Amplitude, false),
  ];
  server.send(bufferTool.Vector3fBuf(2, Scale), client.port, client.address);
}

//Channel point redeem: PosRandomize (Server -> Client)
function PosRandomize(server, client) {
  //Socket ID 3
  const Amplitude = RedeemSet.PosRandomize.Amplitude;
  const NewPos = [
    Rand.Ampli(Amplitude, true),
    Rand.Ampli(Amplitude, false),
    Rand.Ampli(Amplitude, true),
  ];
  server.send(bufferTool.Vector3fBuf(3, NewPos), client.port, client.address);
}

module.exports = {
  outHandler: function (redeemName, server, client) {
    log.log(1, `Sending redeem packet - ${redeemName}`);
    //Switch case through all valid rewards
    switch (redeemName) {
      case "Resize": //Packet ID 2 (Resize)
        Resize(server, client);
        break;
      case "Position Randomizer": //Packet ID 3 (PosRandomize)
        PosRandomize(server, client);
        break;
      case "See Myself Out": //Event ID 1 (PrevScene)
        Events(server, 1, client);
        break;
      case "Change Gravity": //Event ID 2 (GravFlip)
        Events(server, 2, client);
        break;
      case "Up we go": //Event ID 3 (Fling)
        Events(server, 3, client);
        break;
      case "Cappy? Nah": //Event ID 4 (Cappy)
        Events(server, 4, client);
        break;
      case "Random Kingdom": //Event ID 5 (Random Kingdom)
        Events(server, 5, client);
        break;
      case "Highway to Death": //Event ID 6 (CoinTick)
        Events(server, 6, client);
        break;
      case "Whispy Winds": //Event ID 7 (Wind)
        Events(server, 7, client);
        break;
      case "Hot Floor": //Event ID 8
        Events(server, 8, client);
        break;
      case "Stick Flip": //Event ID 9
        Events(server, 9, client);
        break;
      case "Hot Tub Stream": //Event ID 10
        Events(server, 10, client);
        break;
      default:
        //Generic reward
        log.log(2, `Generic reward ${message.rewardTitle} redeemed`);
        break;
    }
  },
};
