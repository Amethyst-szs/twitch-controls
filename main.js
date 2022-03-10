//Initalize server for Starlight
const dgram = require('dgram');
const server = dgram.createSocket('udp4');
let client = {};
//Twitch API requirements
const AuthAPI = require('@twurple/auth');
const PubSub = require('@twurple/pubsub');
const fs = require('fs');
const input = require('input');
//Load in Twitch Auth codes from json
const CurDir = process.cwd();
const Codes = require('./settings/secret.json');
const clientId = Codes.ClientID;
const clientSecret = Codes.Secret;
//Module code
const outPackets = require('./server_bin/outPackets');
const inPackets = require('./server_bin/inPackets');
const log = require('./server_bin/console');
const bufferTool = require('./server_bin/bufferTool');
const redeemHistory = require('./server_bin/recentHandler');
const chalk = require('chalk');

let invalidStage = false;

//Hydration
let Hydrate = parseInt(fs.readFileSync(`${CurDir}/settings/hydrate.txt`).toString().substring(17), 10);

//Respond to packets from the switch
server.on('message', (msg, rinfo) => {
    // console.log(msg);
    switch(msg.readInt8()){
        case -1: //Dummy Initalization
            inPackets.DummyInit(msg, rinfo);
            break;
        case -2: //Initalization
            client = inPackets.Init(msg, rinfo);
            break;
        case -3: //Log any information with msg info
            inPackets.Log(msg, rinfo, CurDir);
            if(bufferTool.disconnectCheck(msg, CurDir)) client = {};
            invalidStage = bufferTool.demoUpdate(msg, CurDir, invalidStage);
            break;
        case -4: //demoToggle
            log.log(0, "DemoToggle");
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

async function getStreamerAuth(){
    console.log(chalk.magentaBright("Please select an account to listen to:"));
    const choices = fs.readdirSync(`${CurDir}/settings/users/`);
    let selection = ``;

    if(choices.length == 0){
        console.log(`No twitch accounts avaliable, please make some JSONs`);
        process.exit();
    } else if (choices.length == 1){
        console.log(chalk.magenta(`Only one user available, selected automatically`));
        selection = choices[0];
    } else {
        selection = await input.select(choices);
    }

    const tokenData = JSON.parse(fs.readFileSync(`${CurDir}/settings/users/${selection}`));
    const authProvider = new AuthAPI.RefreshingAuthProvider(
        {
            clientId,
            clientSecret,
            onRefresh: async newTokenData => await fs.writeFileSync(`${CurDir}/settings/users/${selection}`, JSON.stringify(newTokenData, null, 4))
        },
        tokenData
    );

    return authProvider;
}

//Twitch root function
async function TwitchHandler() {
    //Create an authProvider
    authProvider = await getStreamerAuth();

    //Create a subscription client to the channel point redeems
    const PubSubClient = new PubSub.PubSubClient();
    const userId = await PubSubClient.registerUserListener(authProvider);
    log.log(2, `Subscribed to redeem alerts with channel:read:redemptions scope`);

    //Once Twitch is authenticated and ready, launch UDP server
    server.bind(7902);

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
        
        //Check if the player is currently in a demo scene, if so, STOP
        if(invalidStage){
            log.log(2, `${message.rewardTitle} from ${message.userDisplayName} but the player is in a demo scene`);
            return;
        }

        //Log information about redeem now that we've gotten past the start & update history
        log.log(2, `${message.rewardTitle} - ${message.userDisplayName} redeemed!`);
        redeemHistory.updateList(`${message.rewardTitle} < ${message.userDisplayName}`);

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
            case "Highway to Death": //Event ID 6 (CoinTick)
                outPackets.Events(server, 6, client);
                log.log(1, "Event packet sent (Highway to Death)");
                break;
            case "Whispy Winds": //Event ID 7 (Wind)
                outPackets.Events(server, 7, client);
                log.log(1, "Event packet sent (Whispy Winds)");
                break;
            case "Hot Floor": //Event ID 8
                outPackets.Events(server, 8, client);
                log.log(1, "Event packet sent (Hot Floor)");
                break;
            case "Stick Flip": //Event ID 9
                outPackets.Events(server, 9, client);
                log.log(1, "Event packet sent (Stick Inver)");
                break;
            case "Hot Tub Stream": //Event ID 10
                outPackets.Events(server, 10, client);
                log.log(1, "Event packet sent (Hot Tub Stream)");
                break;
            default: //Generic reward
                log.log(2, `Generic reward ${message.rewardTitle} redeemed`);
                break;
        }
    });
}

TwitchHandler();