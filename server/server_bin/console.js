const chalk = require("chalk");
let nickname = `//////////////`;

module.exports = {
  title: function(title){
    process.stdout.write(
      String.fromCharCode(27) + "]0;" + title + String.fromCharCode(7)
    );
    return;
  },

  setNickname: function(newName){
    nickname = newName;
  },

  log: function(Source, Message) {
    switch (Source) {
      case 0:
        console.log(chalk.greenBright.italic(`[${nickname}] `) + Message);
        break;
      case 1:
        console.log(chalk.yellow.bold("[SERVER] ") + Message);
        break;
      case 2:
        console.log(chalk.magentaBright.bold("[TWITCH] ") + Message);
        break;
      case 3:
        console.log(chalk.blue.bold("[DISCORD] ") + Message);
        break;
      default:
        console.log(chalk.red.bgBlack(`[INVALID] ` + Message));
        break;
    }
  },
};
