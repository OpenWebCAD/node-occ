
const { exec } = require("child_process");

console.log("platform", process.platform);

if (process.platform === "linux") {

    const cmd = "(cd lib/binding && ls *.so.7.2.1 | sed sP^libPPg | sed sp.so.7.2.1pp | xargs -i ln -sf lib{}.so.7.2.1 lib{}.so.7)";

    console.log("cmd = ", cmd);
    exec(cmd, (error, stdout, stderr) => {
        if (error) {
            console.log("failed with err: ", error.code);
        } else {
            console.log("done");
        }
    });

}