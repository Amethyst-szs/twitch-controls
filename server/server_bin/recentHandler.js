const fs = require("fs");
let TargetSize = 3;

module.exports = {
  updateList: function (entry) {
    let RecentRedeems = fs
      .readFileSync(`${process.cwd()}/settings/redeemHistory.txt`)
      .toString()
      .split("\n");

    //If the array of the file isn't the size of target size, reset it
    if (RecentRedeems.length != TargetSize + 1)
      RecentRedeems = ["Empty for now!", "Empty for now!", "Empty for now!"];

    //Cut out the oldest and add the new entry
    RecentRedeems.splice(0, 1);
    RecentRedeems.push(entry);

    //If an empty line slipped in, remove it!
    for (i = 0; i < RecentRedeems.length; i++) {
      if (RecentRedeems[i] == ``) RecentRedeems.splice(i, 1);
    }

    //Create a string to write to the file, not an array
    ExportString = ``;
    for (i = 0; i < TargetSize; i++) {
      ExportString += `${RecentRedeems[i]}\n`;
    }

    //Write the file
    fs.writeFileSync(
      `${process.cwd()}/settings/redeemHistory.txt`,
      ExportString
    );
  },
};
