import { __decorate } from "tslib";
import { DataObject, rawDataSymbol, rtfm } from '@twurple/common';
/**
 * A message that informs about a user redeeming a custom channel points reward.
 */
let PubSubRedemptionMessage = class PubSubRedemptionMessage extends DataObject {
    /**
     * The internal redemption ID.
     */
    get id() {
        return this[rawDataSymbol].data.redemption.id;
    }
    /**
     * The ID of the user that redeemed the reward.
     */
    get userId() {
        return this[rawDataSymbol].data.redemption.user.id;
    }
    /**
     * The name of the user that redeemed the reward.
     */
    get userName() {
        return this[rawDataSymbol].data.redemption.user.login;
    }
    /**
     * The display name of the user that redeemed the reward.
     */
    get userDisplayName() {
        return this[rawDataSymbol].data.redemption.user.display_name;
    }
    /**
     * The ID of the channel where the reward was redeemed.
     */
    get channelId() {
        return this[rawDataSymbol].data.redemption.channel_id;
    }
    /**
     * The date when the reward was redeemed.
     */
    get redemptionDate() {
        return new Date(this[rawDataSymbol].data.redemption.redeemed_at);
    }
    /**
     * The ID of the reward.
     */
    get rewardId() {
        return this[rawDataSymbol].data.redemption.reward.id;
    }
    /**
     * The title of the reward.
     */
    get rewardTitle() {
        return this[rawDataSymbol].data.redemption.reward.title;
    }
    /**
     * The prompt of the reward.
     */
    get rewardPrompt() {
        return this[rawDataSymbol].data.redemption.reward.prompt;
    }
    /**
     * The cost of the reward, in channel points.
     */
    get rewardCost() {
        return this[rawDataSymbol].data.redemption.reward.cost;
    }
    /**
     * Whether the reward gets added to the request queue.
     */
    get rewardIsQueued() {
        return !this[rawDataSymbol].data.redemption.reward.should_redemptions_skip_request_queue;
    }
    /**
     * The image set associated with the reward.
     */
    get rewardImage() {
        return this[rawDataSymbol].data.redemption.reward.image;
    }
    /**
     * The default image set associated with the reward.
     */
    get defaultImage() {
        return this[rawDataSymbol].data.redemption.reward.default_image;
    }
    /**
     * The full message that was sent with the notification.
     */
    get message() {
        return this[rawDataSymbol].data.redemption.user_input;
    }
    /**
     * The status of the redemption.
     */
    get status() {
        return this[rawDataSymbol].data.redemption.status;
    }
};
PubSubRedemptionMessage = __decorate([
    rtfm('pubsub', 'PubSubRedemptionMessage', 'id')
], PubSubRedemptionMessage);
export { PubSubRedemptionMessage };
