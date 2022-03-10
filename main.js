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

    switch(choices.length){
        case 0:
            console.log(`No twitch accounts avaliable, please make some JSONs`);
            process.exit();
        case 1:
            console.log(chalk.magenta(`Only one user available, selected automatically`));
            selection = choices[0];
            break;
        default:
            selection = await input.select(choices);
            break;
    }

    const authProvider = new AuthAPI.RefreshingAuthProvider(
        {
            clientId,
            clientSecret,
            onRefresh: async newTokenData => await fs.writeFileSync(`${CurDir}/settings/users/${selection}`, JSON.stringify(newTokenData, null, 4))
        },
        JSON.parse(fs.readFileSync(`${CurDir}/settings/users/${selection}`))
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

        //Handle redeem in the out packet handler
        outPackets.outHandler(message.rewardTitle);
    });
}

TwitchHandler();