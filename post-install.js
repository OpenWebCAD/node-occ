
const { exec }= require("child_process");


if (process.platform === "linux") {

    const cmd = "(cd lib/binding && ls *.so.7.2.1 | sed sP^libPPg | sed sp.so.7.2.1pp | xargs -i ln -sf lib{}.so.7.2.1 lib{}.so.7)";
    
    exec(cmd, (error, stdout, stderr)  => {
        if (error) {
            console.log(error.code);
        } else {
            console.log("done");
        }
    });

}