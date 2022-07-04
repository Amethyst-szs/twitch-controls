const fs = require('fs');
const twitchInit = require('./twitchInit');
let langInit;
let blockList = [];

const RedeemSet = require("../settings/redeem_set.json");
const eventList = require('../settings/event_list.json').FullEventList;
const spawnList = require('../settings/event_list.json').FullSpawnList;
const Rand = require("./advancedRand");
const bufferTool = require("./bufferTool");
const log = require("./console");

let serverRef;
let client;

//Channel point redeem: PrevScene (Server -> Client)
function Events(EventID, isTwitch) {
  //Socket ID 1 - Event ID 1
  log.debugLog(`Sending event packet with ID ${EventID}`);
  serverRef.send(bufferTool.GenericBuf(1, EventID, isTwitch), client.port, client.address);
}

//Channel point redeem: Resize (Server -> Client)
function Resize(multi) {
  //Socket ID 2
  const Amplitude = RedeemSet.Resize.Amplitude;
  const Scale = [
    Rand.Ampli(Amplitude*multi, false),
    Rand.Ampli(Amplitude*multi, false),
    Rand.Ampli(Amplitude*multi, false),
  ];
  serverRef.send(bufferTool.Vector3fBuf(2, Scale), client.port, client.address);
}

//Channel point redeem: PosRandomize (Server -> Client)
function PosRandomize(multi) {
  //Socket ID 3
  const Amplitude = RedeemSet.PosRandomize.Amplitude;
  const NewPos = [
    Rand.Ampli(Amplitude*multi, true),
    Rand.Ampli(Amplitude*multi, false),
    Rand.Ampli(Amplitude*multi, true),
  ];
  serverRef.send(bufferTool.Vector3fBuf(3, NewPos), client.port, client.address);
}

module.exports = {
  getClient: function(){
    return client;
  },
  
  setClient: function(newClient, nickname){
    client = newClient;
    client.nickname = nickname;
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

  pushNewBlock: function(block){
    blockList.push(block);
    return;
  },

  blockPacket: function(rinfo){
    serverRef.send(new Buffer.alloc(1, 0x06), rinfo.port, rinfo.address);
    return;
  },

  getBlockList: function(){
    return blockList;
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

  toggleMusic: function(){
    log.debugLog("Music toggle step 1");
    if(client)
      Events(15, false);
  },

  prepareLang: function(){
    langInit = JSON.parse(fs.readFileSync(`${process.cwd()}/settings/localize/${twitchInit.getLang()}_init.json`));
    return;
  },
  
  outHandlerId: function(id){
    Events(id, false);
    return;
  },

  outAnyEventHandler: function(eventName, eventType){
    if(client){
      switch(eventType){
        case "Event":
          log.log(3, `Sending event ${eventName} / ID ${eventList.indexOf(eventName)+1}`);
          Events(eventList.indexOf(eventName)+1, false);
          break;
        case "Spawn":
          log.log(3, `Sending event ${eventName} / ID ${spawnList.indexOf(eventName)+51}`);
          Events(spawnList.indexOf(eventName)+51, false);
      }
      return;
    }
    log.log(3, `No client to send event to`);
    return;
  },

  outHandler: function (redeemName, isTwitch) {
    //Get the lang init so you can fetch the original name of the redeem
    redeemTitle = redeemName;

    log.debugLog(`Preparing to send packet with name ${redeemTitle}`);

    //If this is a Twitch log this extra info
    if(!isTwitch)
      log.log(1, `Sending fake redeem packet - ${redeemName}`);

    //If this is a translation, convert back to English
    if(langInit.hasOwnProperty(redeemName))
      redeemTitle = langInit[redeemName].original;

    log.debugLog(`Redeem title after translation ${redeemTitle}`);

    //Switch case through all valid rewards
    switch (redeemTitle) {
      case "Resize": //Packet ID 2 (Resize)
        Resize(1);
        break;
      case "Position Randomizer": //Packet ID 3 (PosRandomize)
        PosRandomize(1);
        break;
      case "Resize DX": //Packet ID 2 (Resize)
        Resize(5);
        break;
      case "Position Randomizer DX": //Packet ID 3 (PosRandomize)
        PosRandomize(5);
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
      case "Moon Magnet": //Event ID 12
        Events(11, isTwitch);
        break;
      case "Fly me to the Moon": //Event ID 13
        Events(12, isTwitch);
        break;
      case "kill": //Event ID 15
        Events(13, isTwitch);
        break;
      case "Pay to Win": //Event ID 16
        Events(14, isTwitch);
        break;
      default:
        //Generic reward
        log.log(1, `Generic reward ${redeemName} redeemed`);
        break;
    }
  },

  outVoiceHandler: function (redeemName) {
    //Get the lang init so you can fetch the original name of the redeem
    redeemTitle = redeemName;
    log.log(4, `You said ${redeemTitle}`);

    if(!client)
      return;

    //Switch case through all valid rewards
    switch (redeemTitle) {
      case "Change Gravity":
        Events(2, false);
        break;
      case "Up Go":
        Events(3, false);
        break;
      case "Cappy":
        Events(4, false);
        break;
      case "Dance":
        Events(16, false);
        break;
      case "Nipple":
        Events(17, false);
        break;
      case "Stick":
        Events(9, false);
        break;
      case "Back":
        Events(1, false);
        break;
      case "Explode":
        Events(18, false);
        break;
      case "Coin":
        Events(6, false);
        break;
      case "Moon":
        Events(11, false);
        break;
      case "Fire":
        Events(19, false);
        break;
      case "MassGoomba":
        Events(60, false);
        break;
      case "MassTrex":
        Events(61, false);
        break;
      case "MassUproot":
        Events(62, false);
        break;
      case "MassStatue":
        Events(63, false);
        break;
      case "MassPokio":
        Events(64, false);
        break;
      case "MassEel":
        Events(65, false);
        break;
      case "MassMoeeye":
        Events(66, false);
        break;
      case "MassTank":
        Events(67, false);
        break;
      default:
        //Generic reward
        log.log(1, `Voice command ${redeemName} was run but not sure what this is?`);
        break;
    }
  },
};
