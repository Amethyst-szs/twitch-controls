# Starlight Twitch
A project to link Twitch channel point redemptions to SMO's Starlight. This project is early on in development and also I suck at everything so expect the code to be messy and gross

## How to use

### Server side
On the server side of this application, you will need to connect a twitch application created by your Twitch account. Explaining this process is a real challenge, so I'd recommend following this bot setup guide to get your `Client ID`, `Client Secret`, `Access Token`, and `Refresh Token`! Make sure your application has both the `channel:read:redemptions` and `channel:manage:redemptions` scopes. Guide: https://twurple.js.org/docs/examples/chat/basic-bot.html

Once you have those four tokens, add your ClientID and Secret into a JSON layed out below:  
`settings/secret.json`
```
{
    "ClientID": "place Client ID here",
    "Secret": "place Secret here"
}
```

You'll need to supply the Access Token and Refresh token now, but it's layed out slightly differently. Make a folder in settings called `users` and then you can make a json with any name. You can make as many of these as you'd like for different Twitch accounts! Use the template below to set it up.

`settings/users/AnyName.json`
```
{
    "accessToken": "place accessToken here",
    "refreshToken": "place refreshToken here",
    "scope": [
        "channel:manage:redemptions",
        "channel:read:redemptions",
        "chat:edit",
        "chat:read"
    ]
}
```

Once you have all this in place the server should be able to be run with both npm and node!

### Switch side

You should be able to compile the project with the make file on Linux or WSL, only part you'll have to change is the local ip address of the server. Just open up `starlight/include/ipAddress.hpp` and supply the server's local IP. This does require both the server and the switch to be on the same network connection.  
  
If you wish to add your own redeems, look into the packet and server code! I can help out if needed

## Credits

*CraftyBoss* - Made the starlight base that I used, gave some hooks and references to help out!
*Fruityloops* - Massive help with everything, especially the UDP packet server code, plus taught me how to make my own hooks 
*MoeagaruYuuki#0001* - Solved my pain trying to work with the Twitch API  
*GRAnimated* - Help with code, created primitive shape drawer  
*Bryce\_\_\_\_\_* - Made the original Twitch Controls "!bonk" giving me the idea to make this  
*Everyone* in the SMO Modding Hub discord who has helped with this specific project or just Starlight in general! Y'all are the best!
