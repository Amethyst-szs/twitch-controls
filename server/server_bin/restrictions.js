const { FullRedeemList } = require("../settings/redeem_list.json");
const restrictionTiers = require("../settings/redeem_restrictions.json");
let allDisabled = false;
let restrictionList = new Array(FullRedeemList.length).fill(false, 0, FullRedeemList.length);
let forcedRestrictionList = new Array(FullRedeemList.length).fill(false, 0, FullRedeemList.length);

module.exports = {
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

    updateStandardRestriction: function(tier){
        //Reset the list
        restrictionList.fill(false, 0, restrictionList.length);
        let targetList = [];

        //Create list of redeems within and below your current restriction tier
        for(i=tier;i>0;i--){
            targetList = targetList.concat(restrictionTiers[i]);
        };

        //Process which of the full redeem list redeems are in the restricted target list
        for(entry in FullRedeemList){
            if(targetList.includes(FullRedeemList[entry]))
                restrictionList[entry] = true;
        }

        const twitchInit = require('./twitchInit');
        twitchInit.skipRefreshTimer();
        return;
    },

    updateForcedRestriction: function(redeemName){
        location = FullRedeemList.indexOf(redeemName);
        forcedRestrictionList[location] = !forcedRestrictionList[location];
        return;
    }
}