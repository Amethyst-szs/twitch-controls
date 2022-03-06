//Initalize server for Starlight
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
let client = {};
//Twitch API requirements
const AuthAPI = require('@twurple/auth');
const PubSub = require('@twurple/pubsub');
const fs = require('fs');
//Load in Twitch Auth codes from json
const CurDir = process.cwd();
const Codes = require('./settings/secret.json');
const clientId = Codes.ClientID;
const clientSecret = Codes.Secret;
//Module code
const outPackets = require('./server_bin/outPackets');
const inPackets = require('./server_bin/inPackets');
const log = require('./server_bin/console');

let invalidStage = false;

//Hydration
let Hydrate = parseInt(fs.readFileSync(`${CurDir}/settings/hydrate.txt`).toString().substring(17), 10);

//Respond to packets from the switch
server.on('message', (msg, rinfo) => {
    console.log(msg);
    switch(msg.readInt8()){
        case -1: //Dummy Initalization
            inPackets.DummyInit(msg, rinfo);
            break;
        case -2: //Initalization
            client = inPackets.Init(msg, rinfo);
            break;
        case -3: //Log any information with msg info
            shouldDisconnect = inPackets.Log(msg, rinfo, CurDir);
            if(shouldDisconnect){
                client = {};
                log.log(1, "Client has requested a disconnect, clearing...");
            }
            break;
    }
});

//Start listening as a server
server.on('listening', () => {
    const address = server.address();
    log.log(1, `Server listening on ${address.port}`);
});

//Close server on error
server.on('error', (err) => {
    log.log(1, `Server error:\n${err.stack}`);
    server.close();
});

//Bind to server IP!
server.bind(7902);

//Twitch root function
async function TwitchHandler() {
    //Create an authProvider
    const tokenData = JSON.parse(fs.readFileSync(`${CurDir}/settings/tokens.json`));
    const authProvider = new AuthAPI.RefreshingAuthProvider(
        {
            clientId,
            clientSecret,
            onRefresh: async newTokenData => await fs.writeFileSync(`${CurDir}/settings/tokens.json`, JSON.stringify(newTokenData, null, 4))
        },
        tokenData
    );

    //Create a subscription client to the channel point redeems
    const PubSubClient = new PubSub.PubSubClient();
    const userId = await PubSubClient.registerUserListener(authProvider);
    log.log(2, `Subscribed to redeem alerts with channel:read:redemptions scope`);
    //Create listener that is triggered every channel point redeem
    const listener = await PubSubClient.onRedemption(userId, (message) => {
        //Dydration
        switch(message.rewardTitle){
            case "Hydrate!":
                Hydrate = parseInt(fs.readFileSync(`${CurDir}/settings/hydrate.txt`).toString().substring(17), 10);
                Hydrate++;
                fs.writeFileSync(`${CurDir}/settings/hydrate.txt`, `Hydration Count: ${Hydrate.toString(10)}`);
                break;
            case "Dehydrate":
                Hydrate = parseInt(fs.readFileSync(`${CurDir}/settings/hydrate.txt`).toString().substring(17), 10);
                Hydrate--;
                fs.writeFileSync(`${CurDir}/settings/hydrate.txt`, `Hydration Count: ${Hydrate.toString(10)}`);
                break;
            case "I Drank Water":
                Hydrate = parseInt(fs.readFileSync(`${CurDir}/settings/hydrate.txt`).toString().substring(17), 10);
                Hydrate--;
                fs.writeFileSync(`${CurDir}/settings/hydrate.txt`, `Hydration Count: ${Hydrate.toString(10)}`);
                break;
        }

        //Check and make sure a switch has connected already
        if(client.address == undefined){
            log.log(2, `${message.rewardTitle} from ${message.userDisplayName} but no client connected yet!`);
            return;
        }

        //Log information about redeem now that we've gotten past the start
        log.log(2, `${message.rewardTitle} - ${message.userDisplayName} redeemed!`);

        //Switch case through all valid rewards
        switch(message.rewardTitle){
            case "Resize": //Packet ID 2 (Resize)
                outPackets.Resize(server, client);
                log.log(1, "Resize packet sent");
                break;
            case "Position Randomizer": //Packet ID 3 (PosRandomize)
                outPackets.PosRandomize(server, client);
                log.log(1, "Position Randomizer packet sent");
                break;
            case "See Myself Out": //Event ID 1 (PrevScene)
                outPackets.Events(server, 1, client);
                log.log(1, "Event packet sent (See Myself Out)");
                break;
            case "Change Gravity": //Event ID 2 (GravFlip)
                outPackets.Events(server, 2, client);
                log.log(1, "Event packet sent (Change Gravity)");
                break;
            case "Up we go": //Event ID 3 (Fling)
                outPackets.Events(server, 3, client);
                log.log(1, "Event packet sent (Up we go)");
                break;
            case "Cappy? Nah": //Event ID 4 (Cappy)
                outPackets.Events(server, 4, client);
                log.log(1, "Event packet sent (Cappy? Nah)");
                break;
            case "Random Kingdom": //Event ID 5 (Random Kingdom)
                outPackets.Events(server, 5, client);
                log.log(1, "Event packet sent (Random Kingdom)");
                break;
            default: //Generic reward
                log.log(2, `Generic reward ${message.rewardTitle} redeemed`);
                break;
        }
    });
}

TwitchHandler();