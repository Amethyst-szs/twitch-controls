//Initalize server for Starlight
const dgram = require("dgram");
const server = dgram.createSocket("udp4");
let client = {};
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
let rejectionList = require("./settings/rejectionListBase.json");

let FullRedeemList = [];
const restrictions = require("./server_bin/restrictions");

//Respond to packets from the switch
server.on("message", (msg, rinfo) => {
  switch (msg.readInt8()) {
    case -1: //Dummy Initalization
      inPackets.DummyInit(msg, rinfo);
      break;
    case -2: //Initalization
      client = inPackets.Init(msg, rinfo);
      rejectionID = 0;
      rejectionList = require("./settings/rejectionListBase.json");
      break;
    case -3: //Log any information with msg info
      //Inital log
      inPackets.Log(msg, rinfo, CurDir);

      //Check if this log is a disconnection request
      if (bufferTool.disconnectCheck(msg, CurDir)) client = {};

      //Invalid stage log type check
      invalidStage = bufferTool.demoUpdate(msg, CurDir, invalidStage);

      //Rejection status update check
      isReject = bufferTool.reject(msg, CurDir);
      if (isReject.wasRejectLog) {
        if (!isReject.rejectionState) {
          refundRedeem(
            api, streamerID,
            rejectionList[isReject.rejectionID].rewardID,
            rejectionList[isReject.rejectionID].ID
          );
        } else {
          approveRedeem(
            api, streamerID,
            rejectionList[isReject.rejectionID].rewardID,
            rejectionList[isReject.rejectionID].ID
          );
        }
      }

      //Restrict status update
      bufferTool.restrict(msg, CurDir);
      break;
    case -4: //demoToggle
      log.log(0, "DemoToggle");
      break;
  }
});

//Start listening as a server
server.on("listening", () => {
  const address = server.address();
  log.log(1, `Server listening on ${address.port}`);
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

async function refundRedeem(api, streamerID, rewardId, id) {
  redemption = await api.channelPoints.getRedemptionById(
    streamerID,
    rewardId,
    id
  );
  if ((await redemption).isFulfilled || (await redemption).isCanceled) {
    log.log(1, `Redeem ${id} came back from the past`);
    return;
  }
  await api.channelPoints.updateRedemptionStatusByIds(
    streamerID,
    rewardId,
    id,
    "CANCELED"
  )
  .catch(console.error);
  return;
}

async function approveRedeem(api, streamerID, rewardId, id) {
  redemption = await api.channelPoints.getRedemptionById(
    streamerID,
    rewardId,
    id
  );
  if ((await redemption).isFulfilled || (await redemption).isCanceled) {
    log.log(1, `Redeem ${id} came back from the past`);
    return;
  }
  await api.channelPoints.updateRedemptionStatusByIds(
    streamerID,
    rewardId,
    id,
    "FULFILLED"
  )
  .catch(console.error);
  return;
}

async function viewerUpdate(streamerMe) {
  let stream = await streamerMe.getStream();
  console.log(stream.viewers);
}

async function backupCheck(api, streamerID, listID) {
  let redeemInfo = await api.channelPoints.getRedemptionById(
    streamerID,
    rejectionList[listID].rewardID,
    rejectionList[listID].ID
  )
  .catch(console.error);
  let isAlreadyDone =
    (await redeemInfo).isFulfilled || (await redeemInfo).isCanceled;
  if (!isAlreadyDone) {
    await redeemInfo.updateStatus("CANCELED")
    .catch(console.error);
  }
}

//Twitch root function
async function TwitchHandler() {
  process.stdout.write(
    String.fromCharCode(27) + "]0;" + `Title Test` + String.fromCharCode(7)
  );
  //Create an authProvider and API access client
  let authProvider = await getStreamerAuth();
  api = new BaseAPI.ApiClient({ authProvider });

  // //Collect some data about the streamer
  let streamerMe = await api.users.getMe(false); //The argument here is to NOT grab the streamer's email!
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
      await twitchInit.Main(api, streamerID, CurDir, langType, true);
      break;
    case "Remove Redeems":
      await twitchInit.Main(api, streamerID, CurDir, langType, false);
      log.log(1, `Thank you!`);
      process.exit();
  }

  //Create a subscription client to the channel point redeems
  const PubSubClient = new PubSub.PubSubClient();
  const userId = await PubSubClient.registerUserListener(authProvider);
  log.log(
    2,
    `Subscribed to redeem alerts with channel:read:redemptions scope!\nWelcome ${streamerMe.displayName}`
  );

  //Starts a timer tracking automatic refreshing of costs, cooldowns, and enabled status
  twitchInit.startRefreshTimer(api, streamerID, streamerMe, CurDir);

  //Once Twitch is authenticated and ready, finish UDP server
  server.bind(7902);

  //Once both the UDP server and Twitch is ready, launch discord bot
  discordHandler.slashCommandInit(CurDir);
  discordHandler.botManage();

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
      log.log(
        2,
        `${message.rewardTitle} from ${message.userDisplayName} but the player is in a demo scene`
      );
      refundRedeem(api, streamerID, message.rewardId, message.id);
      return;
    }

    //If a redeem that is the the restriction list is redeemed, refund it!
    if (restrictions.getRestrictedRedeems().includes(message.rewardTitle)) {
      log.log(
        2,
        `${message.rewardTitle} from ${message.userDisplayName} was redeemed even though it was restricted! Refunded, no big deal!`
      );
      refundRedeem(api, streamerID, message.rewardId, message.id);
      return;
    }

    //Check and make sure a switch has connected already
    if (client.address == undefined) {
      log.log(
        2,
        `${message.rewardTitle} from ${message.userDisplayName} but no client connected yet!`
      );
      refundRedeem(api, streamerID, message.rewardId, message.id);
      return;
    }

    //Log information about redeem now that we've gotten past the start & update history
    log.log(2, `${message.rewardTitle} - ${message.userDisplayName} redeemed!`);
    redeemHistory.updateList(
      `${message.rewardTitle} < ${message.userDisplayName}`
    );

    rejectionID++;
    if (rejectionID >= 10) rejectionID = 1;
    rejectionList[rejectionID].rewardID = message.rewardId;
    rejectionList[rejectionID].ID = message.id;
    tempVal = rejectionID;
    setTimeout(backupCheck, 1500, api, streamerID, tempVal);

    //Handle redeem in the out packet handler
    outPackets.outHandler(message.rewardTitle, server, client, CurDir);
  });
}

TwitchHandler();
