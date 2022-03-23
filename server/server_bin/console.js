const chalk = require("chalk");

module.exports = {
  title: function(title){
    process.stdout.write(
      String.fromCharCode(27) + "]0;" + title + String.fromCharCode(7)
    );
    return;
  },

  log: function(Source, Message) {
    switch (Source) {
      case 0:
        console.log(chalk.green.bold("[CLIENT] ") + Message);
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
