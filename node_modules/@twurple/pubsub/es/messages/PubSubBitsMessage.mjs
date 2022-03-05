import { __decorate } from "tslib";
import { DataObject, rawDataSymbol, rtfm } from '@twurple/common';
/**
 * A message that informs about bits being used in a channel.
 */
let PubSubBitsMessage = class PubSubBitsMessage extends DataObject {
    /**
     * The ID of the user that sent the bits.
     */
    get userId() {
        return this[rawDataSymbol].data.user_id;
    }
    /**
     * The name of the user that sent the bits.
     */
    get userName() {
        return this[rawDataSymbol].data.user_name;
    }
    /**
     * The full message that was sent with the bits.
     */
    get message() {
        return this[rawDataSymbol].data.chat_message;
    }
    /**
     * The number of bits that were sent.
     */
    get bits() {
        return this[rawDataSymbol].data.bits_used;
    }
    /**
     * The total number of bits that were ever sent by the user in the channel.
     */
    get totalBits() {
        return this[rawDataSymbol].data.total_bits_used;
    }
    /**
     * Whether the cheer was anonymous.
     */
    get isAnonymous() {
        return this[rawDataSymbol].data.is_anonymous;
    }
};
PubSubBitsMessage = __decorate([
    rtfm('pubsub', 'PubSubBitsMessage', 'userId')
], PubSubBitsMessage);
export { PubSubBitsMessage };
