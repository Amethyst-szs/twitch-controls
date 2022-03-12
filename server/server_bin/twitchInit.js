const BaseAPI = require("@twurple/api");
const AuthAPI = require("@twurple/auth");
const log = require("./console");

async function createAlreadyExistsList(redeemInits, twitchRedeems) {
  let preExistIndexList = [];
  //Remove all redeems from the streamer which are included in the redeem init list
  for (curRedeem = 0; curRedeem < twitchRedeems.length; curRedeem++) {
    //Run through each of the setup redeems
    for (curSetup = 1; curSetup <= redeemInits.FullList.length; curSetup++) {
      if (twitchRedeems[curRedeem].title == redeemInits[curSetup].title)
        preExistIndexList.push(curRedeem);
    }
  }
  log.log(1, `Collected list of SMO point redeems on channel!`);
  return preExistIndexList;
}

async function deleteOldRedeems(
  streamerID,
  twitchRedeems,
  preExistIndexList,
  api
) {
  //Now that you have a list of indexes for all point redeems to remove, go remove them
  for (i = 0; i < preExistIndexList.length; i++) {
    if (preExistIndexList == 0) continue;
    await api.channelPoints.deleteCustomReward(
      streamerID,
      twitchRedeems[preExistIndexList[i]].id
    );
    log.log(
      2,
      `Cleared ${twitchRedeems[preExistIndexList[i]].title}! (${
        twitchRedeems[preExistIndexList[i]].id
      })`
    );
  }
  log.log(1, `Completed clearing!`);
}

async function publishNewRedeems(streamerID, redeemInits, api) {
  //Publish redeems back to API!
  for (i = 1; i <= redeemInits.FullList.length; i++) {
    await api.channelPoints.createCustomReward(streamerID, redeemInits[i]);
    log.log(
      2,
      `Created new redeem ${redeemInits[i].title}! (ID is not saved!)`
    );
  }
  log.log(1, `Completed publishing!`);
}

module.exports = {
  Main: async function (api, streamerID) {
    //Create inital variables
    const redeemInits = require("../settings/redeem_init.json");
    let twitchRedeems = await api.channelPoints.getCustomRewards(
      streamerID,
      false
    );
    let preExistIndexList = await createAlreadyExistsList(
      redeemInits,
      twitchRedeems
    );

    //Attempt to remove any prexisting redeems with overlapping names
    if (preExistIndexList.length > 0)
      await deleteOldRedeems(streamerID, twitchRedeems, preExistIndexList, api);

    //Publish new redeems to Twitch
    await publishNewRedeems(streamerID, redeemInits, api);
    return;
  },
};
