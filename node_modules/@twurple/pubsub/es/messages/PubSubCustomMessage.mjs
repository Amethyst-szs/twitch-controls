import { __decorate } from "tslib";
import { DataObject, rawDataSymbol, rtfm } from '@twurple/common';
/**
 * A message in response to a subscription to a custom topic.
 */
let PubSubCustomMessage = class PubSubCustomMessage extends DataObject {
    /**
     * The message data.
     */
    get data() {
        return this[rawDataSymbol];
    }
};
PubSubCustomMessage = __decorate([
    rtfm('pubsub', 'PubSubCustomMessage')
], PubSubCustomMessage);
export { PubSubCustomMessage };
