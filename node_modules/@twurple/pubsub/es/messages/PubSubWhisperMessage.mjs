import { __decorate } from "tslib";
import { DataObject, rawDataSymbol, rtfm } from '@twurple/common';
/**
 * A message informing about a whisper being received from another user.
 */
let PubSubWhisperMessage = class PubSubWhisperMessage extends DataObject {
    /**
     * The message text.
     */
    get text() {
        return this[rawDataSymbol].data_object.body;
    }
    /**
     * The ID of the user who sent the whisper.
     */
    get senderId() {
        return this[rawDataSymbol].data_object.from_id.toString();
    }
    /**
     * The name of the user who sent the whisper.
     */
    get senderName() {
        return this[rawDataSymbol].data_object.tags.login;
    }
    /**
     * The display name of the user who sent the whisper.
     */
    get senderDisplayName() {
        return this[rawDataSymbol].data_object.tags.display_name;
    }
};
PubSubWhisperMessage = __decorate([
    rtfm('pubsub', 'PubSubWhisperMessage', 'senderId')
], PubSubWhisperMessage);
export { PubSubWhisperMessage };
