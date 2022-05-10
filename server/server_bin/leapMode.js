//Modules
const bufferTool = require("./bufferTool");

//Identification
let serverRef;
let client;

////Leap state

//Left Hand

//Right Hand
let leapRight = {
    "grab": false,
    "position": {
        "x": 0,
        "y": 0,
        "z": 0
    },
    "velocity": {
        "x": 0,
        "y": 0,
        "z": 0
    }
}

module.exports = {
    init: function(server){
        //Set the server
        serverRef = server;
        setInterval(updateLeap, 100);
    },
    setClient: function(newClient){
        client = newClient;
        return;
    }
}

async function updateLeap(){
    //Calculate position and velocity


    if(client){
        serverRef.send(bufferTool.GenericBuf(1, 2, false), client.port, client.address);
    }
}