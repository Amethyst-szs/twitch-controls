const BaseAPI = require("@twurple/api");
const AuthAPI = require("@twurple/auth");
const log = require("./console");
const fs = require('fs');

async function createAlreadyExistsList(redeemInits, twitchRedeems) {
  let preExistIndexList = [];

  for (var redeem in twitchRedeems){
    if(redeemInits.hasOwnProperty(twitchRedeems[redeem].title))
      preExistIndexList.push(redeem);
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
  for (redeem in redeemInits) {
    console.log(redeem, redeemInits[redeem]);
    await api.channelPoints.createCustomReward(streamerID, redeemInits[redeem]);
    log.log(
      2,
      `Created new redeem ${redeemInits[redeem].title}! (ID is not saved!)`
    );
  }
  log.log(1, `Completed publishing!`);
}

async function updateRedeemCost(twitchRedeem, redeem, api, streamerID){
  if(redeem!=null){
    log.log(2, `Updated pricing of ${redeem.title}`);
    await api.channelPoints.updateCustomReward(streamerID, twitchRedeem.id, redeem);
  }
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

  priceUpdate: async function (api, streamerID, streamerMe, CurDir, factor) {
    //Get a list of Twitch Controls redeems
    const redeemInits = JSON.parse(fs.readFileSync(`${CurDir}/settings/redeem_init.json`));
    let twitchRedeems = await api.channelPoints.getCustomRewards(
      streamerID,
      false
    );
    let preExistIndexList = await createAlreadyExistsList(
      redeemInits,
      twitchRedeems
    );

    let stream = await streamerMe.getStream();
    let viewers = stream.viewers;

    if(viewers == null)
      return;

    //Update price listing
    for (twitchListing = 0; twitchListing < preExistIndexList.length; twitchListing++) {
      let twitchRedeem = twitchRedeems[preExistIndexList[twitchListing]]
      let title = twitchRedeem.title;
      let redeem = redeemInits[title];
      redeem.cost *= factor*Math.max(1,Math.log10(viewers)/2);
      redeem.cost = Math.floor(redeem.cost/10)*10;
      
      if(redeem.cost==0)
        redeem.cost = 10;

      if(redeem.cost!=twitchRedeem.cost)
        setTimeout(updateRedeemCost, (twitchListing+1)*1000, twitchRedeem, redeem, api, streamerID);
    }
    log.log(1, `Finished queuing price updates!`);
  },
};
