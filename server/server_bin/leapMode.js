//Modules
const bufferTool = require("./bufferTool");
// const Leap = require('leapjs');

//Identification
let serverRef;
let client;

async function updateLeap(){
    //Calculate position and velocity
    if(client){
        serverRef.send(bufferTool.GenericBuf(1, 2, false), client.port, client.address);
    }
}

module.exports = {
    init: function(server){
        //Set the server
        serverRef = server;
        // Leap.loop({
        //     // frame callback is run before individual frame components
        //     frame: function(frame){
                
        //     },

        //     // hand callbacks are run once for each hand in the frame
        //     hand: function(hand){
        //         console.log( "Hand: " + hand.id + ' &nbsp;roll: ' + Math.round(hand.roll() * TO_DEG) + '°<br/>');
        //     }
        // });
        setInterval(updateLeap, 100);
    },
    setClient: function(newClient){
        client = newClient;
        return;
    }
}