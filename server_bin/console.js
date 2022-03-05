const chalk = require('chalk');

module.exports = {
    log(Source, Message){
        switch(Source){
            case 0:
                console.log(chalk.green.bold("[CLIENT] ") + Message);
                break;
            case 1:
                console.log(chalk.cyanBright.bold("[SERVER] ") + Message);
                break;
            case 2:
                console.log(chalk.magentaBright.bold("[TWITCH] ") + Message);
                break;
            default:
                console.log(`[INVALID] ` + Message);
                break;
        }
    }
}