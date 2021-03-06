let FullRedeemList = [];

const restrictionTiers = require("../settings/redeem_restrictions.json");
const fs = require('fs');

let restrictionTier = 0;

let allDisabled = false;
let restrictionList = [];
let forcedRestrictionList = [];

module.exports = {
    setupRedeemList: function(CurDir, lang){
        FullRedeemList = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${langType}_list.json`)).FullRedeemList;
        restrictionList = new Array(FullRedeemList.length).fill(false, 0, FullRedeemList.length);
        forcedRestrictionList = new Array(FullRedeemList.length).fill(false, 0, FullRedeemList.length);

        //Disable vaulted redeems
        let vault = JSON.parse(fs.readFileSync(`${CurDir}/settings/redeem_vault.json`)).vault;
        for(entry in vault){
            this.updateForcedRestriction(vault[entry]);
        }

        return;
    },

    getRestrictedTier: function(){
        return restrictionTier;
    },

    getRestrictedRedeems: function(){
        let returnList = [];
        for(entry in FullRedeemList){
            if(restrictionList[entry] || forcedRestrictionList[entry])
                returnList.push(FullRedeemList[entry]);
        }
        return returnList;
    },
    
    toggleAllDisabled: function(){
        allDisabled = !allDisabled;
        forcedRestrictionList.fill(allDisabled, 0, forcedRestrictionList.length);
        return;
    },

    updateStandardRestriction: function(tier, CurDir){
        restrictionTier = tier;
        //Grab the init information so you can grab the English names regardless of language
        const twitchInit = require('./twitchInit');
        let langList = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${twitchInit.getLang()}_list.json`)).FullRedeemList;
        let langInit = JSON.parse(fs.readFileSync(`${CurDir}/settings/localize/${twitchInit.getLang()}_init.json`));

        //Reset the list
        restrictionList.fill(false, 0, restrictionList.length);
        let targetList = [];

        //Create list of redeems within and below your current restriction tier
        for(i=tier;i>0;i--){
            targetList = targetList.concat(restrictionTiers[i]);
        };

        //Process which of the full redeem list redeems are in the restricted target list
        for(entry in langList){
            if(targetList.includes(langInit[langList[entry]].original))
                restrictionList[entry] = true;
        }

        twitchInit.skipRefreshTimer();
        return;
    },

    updateForcedRestriction: function(redeemName){
        location = FullRedeemList.indexOf(redeemName);
        forcedRestrictionList[location] = !forcedRestrictionList[location];
        return;
    }
}