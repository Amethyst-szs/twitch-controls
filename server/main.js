//Initalize server for Starlight
const dgram = require("dgram");
const server = dgram.createSocket("udp4");
//Twitch API requirements
const BaseAPI = require("@twurple/api");
let api;
const AuthAPI = require("@twurple/auth");
const PubSub = require("@twurple/pubsub");
const fs = require("fs");
const input = require("input");
//Load in Twitch Auth codes from json
const CurDir = process.cwd();
const Codes = require("./settings/secret.json");
const clientId = Codes.ClientID;
const clientSecret = Codes.Secret;
//Discord module code
const discordHandler = require('./discord_bin/handler');
//Module code
const outPackets = require("./server_bin/outPackets");
const inPackets = require("./server_bin/inPackets");
const log = require("./server_bin/console");
const bufferTool = require("./server_bin/bufferTool");
const redeemHistory = require("./server_bin/recentHandler");
const twitchInit = require("./server_bin/twitchInit");
const chalk = require("chalk");

let invalidStage = false;
let streamerID = null;

let rejectionID = 0;
let rejectionListSize = 50;
let rejectionList = [];

let FullRedeemList = [];
const restrictions = require("./server_bin/restrictions");

let lastPingTime = new Date().getTime();
let curTime = new Date().getTime();

//Respond to packets from the switch
server.on("message", (msg, rinfo) => {
  // console.log(msg);
  switch (msg.readInt8()) {
    // case -1: //Dummy Initalization
      //No code is actually ran here
      // break;
    case -2: //Initalization
      nickname = msg.slice(1, msg.indexOf(0x00)).toString().toUpperCase();

      if(!outPackets.getBlockList().includes(nickname)){
        outPackets.setClient(rinfo);
        log.setNickname(nickname);
      
        initRejectionList();
        lastPingTime = new Date().getTime();
        serverPing();

        log.log(0, "New client connected! Initalization was completed successfully!");
      } else {
        outPackets.blockPacket(rinfo);
        console.log("Blocking connection");
      }

      break;
    case -3: //Log any information with msg info
      //Ping check
      if(bufferTool.PingBuf(msg, CurDir)){
        lastPingTime = curTime;
        break;
      }

      //Check if this log is a disconnection request
      if (bufferTool.disconnectCheck(msg, CurDir)){
        outPackets.clearClient();
        break;
      }

      //Invalid stage log type check
      if(bufferTool.demoUpdate(msg, CurDir)){
        break;
      };

      //Rejection status update check
      isReject = bufferTool.reject(msg, CurDir);
      if (isReject.wasRejectLog){
        updateRedeem(api, streamerID,
          rejectionList[isReject.rejectionID].rewardID,
          rejectionList[isReject.rejectionID].ID,
          !isReject.rejectionState);
        break;
      }

      //Restrict status update
      if(bufferTool.restrict(msg, CurDir)){
        break;
      };

      inPackets.Log(msg, rinfo, CurDir);
      break;
    case -4: //demoToggle
      log.log(0, "DemoToggle");
      break;
  }
});

async function serverPing(){
  curTime = new Date().getTime();
  curClient = outPackets.getClient();

  if(curClient){
    const buf = Buffer.alloc(1);
    buf.writeInt8(4, 0);
    server.send(buf, curClient.port, curClient.address);
  }

  if((curTime - lastPingTime >= 6500 && curClient) || (outPackets.getBlockList().includes(log.getNickname()))){
    log.log(1, `\n///\nCONNECTION TO CLIENT WAS LOST!!\n///\n`);
    outPackets.clearClient();
    log.setNickname("//////////////");
    initRejectionList();
  }
}

//Start listening as a server
server.on("listening", () => {
  const address = server.address();
  log.log(1, `Server listening on ${address.port}`);
  setInterval(serverPing, 2500);
});

//Close server on error
server.on("error", (err) => {
  log.log(1, `Server error:\n${err.stack}`);
  server.close();
});

async function getStreamerAuth() {
  console.log(chalk.magentaBright("Please select an account to listen to:"));
  const choices = fs.readdirSync(`${CurDir}/settings/users/`);
  let selection = ``;

  switch (choices.length) {
    case 0:
      console.log(`No twitch accounts avaliable`);
      process.exit();
    case 1:
      console.log(
        chalk.magenta(`Only one user available, selected automatically`)
      );
      selection = choices[0];
      break;
    default:
      selection = await input.select(choices);
      break;
  }

  //Get streamer auth and update the cost factor accordingly
  const streamerAuth = JSON.parse(fs.readFileSync(`${CurDir}/settings/users/${selection}`));

  const authProvider = new AuthAPI.RefreshingAuthProvider(
    {
      clientId,
      clientSecret,
      onRefresh: async (newTokenData) => {
        await fs.writeFileSync(
          `${CurDir}/settings/users/${selection}`,
          JSON.stringify(newTokenData, null, 4)
        );},
    },
    streamerAuth
  );

  return authProvider;
}

async function langSelect(){
  //Get list of language definition files
  let langFiles = fs.readdirSync(`${CurDir}/settings/localize/`);
  let languages = [];

  //Run through this list and chop off details, creating a list of languages WITH DUPLICATES
  for(curLang in langFiles){
    languages.push(langFiles[curLang].slice(0, langFiles[curLang].indexOf(`_`)));
  }

  //Now remove all duplicates
  let uniqueLangs = [...new Set(languages)];

  //Let the user select if more than one language is available
  switch (uniqueLangs.length) {
    case 0:
      console.log(`No languages exist!!`);
      process.exit();
    case 1:
      console.log(
        chalk.magenta(`Only one language exists, skipping language selection`)
      );
      return uniqueLangs[0];
    default:
      return await input.select(uniqueLangs);
  }
}

function initRejectionList(){
  rejectionID = 0;
  rejectionList = [];

  for(i=0;i<rejectionListSize;i++){
    rejectionList.push({
      "rewardID": null,
      "ID": null
    });
  }

  return;
}

async function updateRedeem(api, streamerID, rewardId, id, isRefund) {
  //Grab the redemption, ready to catch an error
  redemption = await api.channelPoints.getRedemptionById(streamerID, rewardId, id)
  .catch(console.error);
  if(!redemption){ //If the redemption wasn't found, return early
    return;
  }

  if ((await redemption).isFulfilled || (await redemption).isCanceled) { //If the redeem was handled elsewhere, skip
    log.log(1, `Redeem ${id} was already handled elsewhere?`);
    return;
  }
  
  //Set the respond based on the isRefund parameter
  response = `FULFILLED`;
  if(isRefund)
    response = `CANCELED`;

  await api.channelPoints.updateRedemptionStatusByIds(streamerID, rewardId, id, response)
  .catch(console.error);
  return;
}

async function backupCheck(api, streamerID, listID) {
  let redeemInfo = await api.channelPoints.getRedemptionById(streamerID, rejectionList[listID].rewardID, rejectionList[listID].ID)
  .catch(console.error);

  if(!redeemInfo){
    return;
  }

  let isAlreadyDone =
    (await redeemInfo).isFulfilled || (await redeemInfo).isCanceled;
  if (!isAlreadyDone) {
    await redeemInfo.updateStatus("CANCELED")
    .catch(console.error);
  }
}

async function testFunc(){
  if(outPackets.getClient()){
    outPackets.outHandler("Hot Tub Stream", false);
  }
  return;
}

//Twitch root function
async function TwitchHandler() {
  log.title(`Launch menu`);
  //Create an authProvider and API access client
  let authProvider = await getStreamerAuth();
  api = new BaseAPI.ApiClient({ authProvider });
  
  // //Collect some data about the streamer
  let streamerMe = await api.users.getMe(false) //The argument here is to NOT grab the streamer's email!
  .catch(console.error);
  
  streamerID = streamerMe.id;

  //Let the client language be selected
  langType = await langSelect();
  twitchInit.setLang(langType);
  FullRedeemList = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${langType}_list.json`)).FullRedeemList;
  restrictions.setupRedeemList(CurDir, langType);

  //Quick menu check to see what the user is looking to do
  runType = await input.select(["Run Server", "Initalize Twitch Account", "Remove Redeems"]);
  switch(runType){
    case "Initalize Twitch Account": 
      log.title(`Initalizing redeems`);
      await twitchInit.Main(api, streamerID, CurDir, langType, true);
      break;
    case "Remove Redeems":
      log.title(`Removing all redeems from account`);
      await twitchInit.Main(api, streamerID, CurDir, langType, false);
      log.log(1, `Thank you!`);
      process.exit();
  }

  //Create a subscription client to the channel point redeems
  const PubSubClient = new PubSub.PubSubClient();
  const userId = await PubSubClient.registerUserListener(authProvider);
  log.log(2, `Subscribed to redeem alerts with channel:read:redemptions scope!\nWelcome ${streamerMe.displayName}` );

  //Starts a timer tracking automatic refreshing of costs, cooldowns, and enabled status
  twitchInit.startRefreshTimer(api, streamerID, streamerMe, CurDir);
  twitchInit.setupS(langType);

  //Once Twitch is authenticated and ready, finish UDP server
  server.bind(7902);
  outPackets.setServerRef(server);
  outPackets.prepareLang();

  //Once both the UDP server and Twitch is ready, launch discord bot
  discordHandler.slashCommandInit(CurDir);
  discordHandler.botManage();

  // setInterval(testFunc, 50);

  //Create listener that is triggered every channel point redeem
  const listener = await PubSubClient.onRedemption(userId, (message) => {
    //Start by verifying the redeem here is actually an SMO point redeem just to avoid altering redeems it can't
    if(!FullRedeemList.includes(message.rewardTitle)){
      log.log(
        2,
        `Generic ${message.rewardTitle} from ${message.userDisplayName}`
      );
      return;
    }

    //Check if the player is currently in a demo scene, if so, STOP
    if (invalidStage) {
      updateRedeem(api, streamerID, message.rewardId, message.id, true);
      return;
    }

    //If a redeem that is the the restriction list is redeemed, refund it!
    if (restrictions.getRestrictedRedeems().includes(message.rewardTitle)) {
      updateRedeem(api, streamerID, message.rewardId, message.id, true);
      return;
    }

    //Check and make sure a switch has connected already
    if (!outPackets.getClient()) {
      updateRedeem(api, streamerID, message.rewardId, message.id, true);
      return;
    }

    //Log information about redeem now that we've gotten past the start & update history
    redeemHistory.updateList(
      `${message.rewardTitle} < ${message.userDisplayName}`
    );

    rejectionID++;
    if (rejectionID >= rejectionListSize)
      rejectionID = 1;
    rejectionList[rejectionID].rewardID = message.rewardId;
    rejectionList[rejectionID].ID = message.id;
    tempVal = rejectionID;
    setTimeout(backupCheck, 1500, api, streamerID, tempVal);
    
    // console.log(rejectionList);

    //Handle redeem in the out packet handler
    outPackets.outHandler(message.rewardTitle, true);
  });
}

TwitchHandler();
