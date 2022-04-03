const BaseAPI = require("@twurple/api");
const AuthAPI = require("@twurple/auth");
const log = require("./console");
// const twitchInit = require('./twitchinit');

const fs = require('fs');
const restrictions = require("./restrictions");

let lang = `english`;

let costFactor = 1;
let costDisabled = false;
let cooldownMulti = 1;

let refreshTotalTimer = 180;
let refreshTimer = 0;

async function createAlreadyExistsList(twitchRedeems, langList) {
  let preExistIndexList = [];

  for (var redeem in twitchRedeems){
    for (curName in langList){
      if(twitchRedeems[redeem].title == langList[curName])
        preExistIndexList.push(redeem);
    }
  }
  log.log(1, `Collected list of SMO point redeems on channel!`);
  return preExistIndexList;
}

async function deleteOldRedeems(streamerID, twitchRedeems, preExistIndexList, api) {
  //Now that you have a list of indexes for all point redeems to remove, go remove them
  for (i = 0; i < preExistIndexList.length; i++) {
    if (preExistIndexList == 0) continue;
    await api.channelPoints.deleteCustomReward(streamerID, twitchRedeems[preExistIndexList[i]].id)
      .catch(console.error);

    log.log(2, `Cleared ${twitchRedeems[preExistIndexList[i]].title}! (${twitchRedeems[preExistIndexList[i]].id})`);
  }
  log.log(1, `Completed clearing!`);
}

async function publishNewRedeems(streamerID, redeemInits, api) {
  //Publish redeems back to API!
  for (redeem in redeemInits) {
    await api.channelPoints.createCustomReward(streamerID, redeemInits[redeem])
      .catch(console.error);

    log.log(2,`Created new redeem ${redeemInits[redeem].title}! (ID is not saved!)`);
  }
  log.log(1, `Completed publishing!`);
}

async function updateRedeemCost(twitchRedeem, redeem, api, streamerID){
  if(redeem != null){
    log.log(2, `Updating status of ${redeem.title}`);

    await api.channelPoints.updateCustomReward(streamerID, twitchRedeem, redeem)
      .catch(console.error);
  }
}

function RefreshTimer(api, streamerID, streamerMe, CurDir){
  log.title(`Refresh:${refreshTimer}Restrict:${restrictions.getRestrictedTier()}`);
  if(refreshTimer <= 0){
    refreshTimer = refreshTotalTimer;
    priceUpdate(api, streamerID, streamerMe, CurDir)
      .catch(console.error);
    
    setTimeout(RefreshTimer, 1000, api, streamerID, streamerMe, CurDir);
  } else {
    refreshTimer--;
    setTimeout(RefreshTimer, 1000, api, streamerID, streamerMe, CurDir);
  }
  return;
}

async function priceUpdate(api, streamerID, streamerMe, CurDir) {
  //Get a list of Twitch Controls redeems
  const redeemInits = JSON.parse(fs.readFileSync(`${CurDir}/settings/redeem_init.json`));
  langList = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${lang}_list.json`)).FullRedeemList;
  langInit = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${lang}_init.json`));

  //Add the lang titles and descriptions into the base init object
  //Yes I'm aware this shit is complicated as hell and probably doesn't need to be
  for(cur in langList){
    redeemInits[langInit[langList[cur]].original].title = langInit[langList[cur]].title;
    redeemInits[langInit[langList[cur]].original].prompt = langInit[langList[cur]].prompt;
  }

  //Grab all the current redeems on the streamer's account
  let twitchRedeems = await api.channelPoints.getCustomRewards(streamerID, false)
    .catch(console.error);
  
  let preExistIndexList = await createAlreadyExistsList(twitchRedeems, langList);

  //Get the streamer's current viewer count
  let stream = await streamerMe.getStream();
  if(stream == null)
    stream = {"viewers": 0}

  let viewers = stream.viewers;
  if(viewers == null)
    viewers = 0;
  
  //Update price listing
  let updatedAmount = 0; //This is how many Twitch redeems got updated in this run
  for (twitchListing = 0; twitchListing < preExistIndexList.length; twitchListing++) {
    let twitchRedeem = twitchRedeems[preExistIndexList[twitchListing]]
    let title = twitchRedeem.title;
    let redeem = redeemInits[langInit[title].original];
    let restrictedRedeems = restrictions.getRestrictedRedeems();

    //If the redeem is invalid for some magic reason, log it and run away
    if(redeem == undefined){
      log.log(1, `Can't update the price of a redeem that doesn't exist??`);
      continue;
    }

    //Handle the dynamic cost
    redeem.cost *= costFactor*Math.max(1,Math.log10(viewers)/2);
    redeem.cost = Math.floor(redeem.cost/10)*10;

    //In these edge cases, fix it
    if(redeem.cost==0)
      redeem.cost = 10;
    if(costDisabled)
      redeem.cost = 1;
    
    //Sets the global cooldown based on the base and cooldown multiplier
    redeem.globalCooldown = Math.floor(redeem.globalCooldown*cooldownMulti);

    //Sets if the redeem is enabled based on the restriction list
    redeem.isEnabled = !restrictedRedeems.includes(title);

    //Check if the cost, global cooldown, or enabled status has changed
    if((redeem.cost != twitchRedeem.cost)
    || (redeem.globalCooldown != twitchRedeem.globalCooldown)
    || (redeem.isEnabled != twitchRedeem.isEnabled)){
      updatedAmount++;
      setTimeout(updateRedeemCost, (updatedAmount+1)*500, twitchRedeem.id, redeem, api, streamerID);
    }
  }
  log.log(1, `Finished queuing price updates!`);
}

module.exports = {
  Main: async function (api, streamerID, CurDir, lang, shouldRestore) {
    //Create inital variables
    const redeemInits = require("../settings/redeem_init.json");
    let langList = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${lang}_list.json`)).FullRedeemList;
    let langInit = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${lang}_init.json`));
  
    //Add the lang titles and descriptions into the base init object
    //Yes I'm aware this shit is complicated as hell and probably doesn't need to be
    for(cur in langList){
      //Perform an error check first
      if(!langInit[langList[cur]]){
        log.log(1, `Redeem #${parseInt(cur, 10)+1} caused an error while creating language, check that everything is correct here!`);
        process.exit();
      }

      if(redeemInits.hasOwnProperty(langInit[langList[cur]].original)){
        redeemInits[langInit[langList[cur]].original].title = langInit[langList[cur]].title;
        redeemInits[langInit[langList[cur]].original].prompt = langInit[langList[cur]].prompt;
      } else {
        log.log(1, `Issue where the key exists but no original tag for Redeem #${cur}`);
        process.exit();
      }
    }

    let twitchRedeems = await api.channelPoints.getCustomRewards(streamerID, false)
      .catch(console.error);

    let preExistIndexList = await createAlreadyExistsList(twitchRedeems, langList);

    //Attempt to remove any prexisting redeems with overlapping names
    if (preExistIndexList.length > 0)
      await deleteOldRedeems(streamerID, twitchRedeems, preExistIndexList, api);

    //Publish new redeems to Twitch
    if(shouldRestore)
      await publishNewRedeems(streamerID, redeemInits, api);
    return;
  },

  disCostUpdate: async function(type, amount){
    //Handle the type
    switch(type){
      case "inc": //Increases the cost by amount
        costFactor += amount;
        break;
      case "dec": //Decreases the cost by amount
        costFactor -= amount;
        break;
      case "set": //Sets the cost to the amount
        costFactor = amount;
        break;
      case "dis": //Toggles the cost disabled bool
        costDisabled = !costDisabled;
        break;
    }

    refreshTimer = 0;
  },

  setLang: async function(newLang){
    lang = newLang;
    return;
  },

  getLang: function(){
    return lang;
  },

  disCooldownUpdate: async function(amount){
    cooldownMulti = amount;
    refreshTimer = 0;
  },

  startRefreshTimer: function(api, streamerID, streamerMe, CurDir){
    RefreshTimer(api, streamerID, streamerMe, CurDir);
    return;
  },

  skipRefreshTimer: function(){
    refreshTimer = 0;
    return;
  }
};
