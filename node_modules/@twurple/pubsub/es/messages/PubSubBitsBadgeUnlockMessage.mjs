import { __decorate } from "tslib";
import { DataObject, rawDataSymbol, rtfm } from '@twurple/common';
/**
 * A message that informs about a user unlocking a new bits badge.
 */
let PubSubBitsBadgeUnlockMessage = class PubSubBitsBadgeUnlockMessage extends DataObject {
    /**
     * The ID of the user that unlocked the badge.
     */
    get userId() {
        return this[rawDataSymbol].data.user_id;
    }
    /**
     * The name of the user that unlocked the badge.
     */
    get userName() {
        return this[rawDataSymbol].data.user_name;
    }
    /**
     * The full message that was sent with the notification.
     */
    get message() {
        return this[rawDataSymbol].data.chat_message;
    }
    /**
     * The new badge tier.
     */
    get badgeTier() {
        return this[rawDataSymbol].data.badge_tier;
    }
};
PubSubBitsBadgeUnlockMessage = __decorate([
    rtfm('pubsub', 'PubSubBitsBadgeUnlockMessage', 'userId')
], PubSubBitsBadgeUnlockMessage);
export { PubSubBitsBadgeUnlockMessage };
