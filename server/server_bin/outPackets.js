const fs = require('fs');
const twitchInit = require('./twitchInit');
let langInit;

const RedeemSet = require("../settings/redeem_set.json");
const Rand = require("./advancedRand");
const bufferTool = require("./bufferTool");
const log = require("./console");

let serverRef;
let client;

//Channel point redeem: PrevScene (Server -> Client)
function Events(EventID, isTwitch) {
  //Socket ID 1 - Event ID 1
  serverRef.send(bufferTool.GenericBuf(1, EventID, isTwitch), client.port, client.address);
}

//Channel point redeem: Resize (Server -> Client)
function Resize() {
  //Socket ID 2
  const Amplitude = RedeemSet.Resize.Amplitude;
  const Scale = [
    Rand.Ampli(Amplitude, false),
    Rand.Ampli(Amplitude, false),
    Rand.Ampli(Amplitude, false),
  ];
  serverRef.send(bufferTool.Vector3fBuf(2, Scale), client.port, client.address);
}

//Channel point redeem: PosRandomize (Server -> Client)
function PosRandomize() {
  //Socket ID 3
  const Amplitude = RedeemSet.PosRandomize.Amplitude;
  const NewPos = [
    Rand.Ampli(Amplitude, true),
    Rand.Ampli(Amplitude, false),
    Rand.Ampli(Amplitude, true),
  ];
  serverRef.send(bufferTool.Vector3fBuf(3, NewPos), client.port, client.address);
}

module.exports = {
  getClient: function(){
    return client;
  },
  
  setClient: function(newClient){
    client = newClient;
    return;
  },

  clearClient: function(){
    client = null;
    return;
  },

  setServerRef: function(server){
    serverRef = server;
    return;
  },

  sayMessage: function(msg){
    // Check that the client is valid
    if(!client){
      log.log(3, `Message can not be sent to client because there is no client`);
      return;
    }

    let breakRate = 92;

    //Calculate line breaks
    for(i=1;i<Math.ceil(msg.length/breakRate);i++){
      msg = msg.substring(0, (i*breakRate)+i+i) + `\n` + msg.substring((i*breakRate)+i+i, msg.length);
    }

    //Handle the weird af multiple of 16 + 1 error
    if((msg.length%16)-1 == 0)
      msg += ` `;

    // bufferTool.sayBuf(msg)
    serverRef.send(bufferTool.sayBuf(msg), client.port, client.address);
  },

  prepareLang: function(){
    langInit = JSON.parse(fs.readFileSync(`${process.cwd()}/settings/localize/${twitchInit.getLang()}_init.json`));
    return;
  },
  
  outHandler: function (redeemName, isTwitch) {
    //Get the lang init so you can fetch the original name of the redeem
    redeemTitle = redeemName;

    //If this is a Twitch log this extra info
    if(isTwitch)
      log.log(1, `Sending redeem packet - ${redeemName}`);

    //If this is a translation, convert back to English
    if(langInit.hasOwnProperty(redeemName))
      redeemTitle = langInit[redeemName].original;

    //Switch case through all valid rewards
    switch (redeemTitle) {
      case "Resize": //Packet ID 2 (Resize)
        Resize();
        break;
      case "Position Randomizer": //Packet ID 3 (PosRandomize)
        PosRandomize();
        break;
      case "See Myself Out": //Event ID 1 (PrevScene)
        Events(1, isTwitch);
        break;
      case "Change Gravity": //Event ID 2 (GravFlip)
        Events(2, isTwitch);
        break;
      case "Up we go": //Event ID 3 (Fling)
        Events(3, isTwitch);
        break;
      case "Cappy? Nah": //Event ID 4 (Cappy)
        Events(4, isTwitch);
        break;
      case "Random Kingdom": //Event ID 5 (Random Kingdom)
        Events(5, isTwitch);
        break;
      case "Highway to Death": //Event ID 6 (CoinTick)
        Events(6, isTwitch);
        break;
      case "Whispy Winds": //Event ID 7 (Wind)
        Events(7, isTwitch);
        break;
      case "Hot Floor": //Event ID 8
        Events(8, isTwitch);
        break;
      case "Stick Flip": //Event ID 9
        Events(9, isTwitch);
        break;
      case "Hot Tub Stream": //Event ID 10
        Events(10, isTwitch);
        break;
      case "Dance Party": //Event ID 11
        Events(11, isTwitch);
        break;
      case "Moon Magnet": //Event ID 12
        Events(12, isTwitch);
        break;
      case "Fly me to the Moon": //Event ID 13
        Events(13, isTwitch);
        break;
      case "unfreeze": //Event ID 14
        Events(14, isTwitch);
        break;
      case "kill": //Event ID 15
        Events(15, isTwitch);
        break;
      default:
        //Generic reward
        log.log(2, `Generic reward ${redeemName} redeemed`);
        break;
    }
  },
};
