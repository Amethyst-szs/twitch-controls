//Modules
const bufferTool = require("./bufferTool");
const Leap = require('leapjs');

//Identification
let serverRef;
let client;

async function mainleap(){
    // ASSIGN LEAP MOTION CONTROLLER
    var controller = new Leap.Controller();
    controller.connect();
    console.log(controller.connection);

    console.log(Leap.version);

    controller.on('streamingStarted', onServiceStreaming);
    function onServiceStreaming()
    {
        console.log("Service started streaming event");
    }

    controller.on('streamingStopped', onServiceStopped);
    function onServiceStopped()
    {
        console.log("Service stopped streaming event");
    }
};

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
        setInterval(updateLeap, 100);
        mainleap();
    },
    setClient: function(newClient){
        client = newClient;
        return;
    }
}