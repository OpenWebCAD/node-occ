const occ = require("../lib/occ");

const pace = require("pace")(1000);

const filename = process.argv[2];
let message;

occ.readSTEP(filename, function (err, solids) {
    pace.op(100000);
    if (!err) {
        console.log(" \n\ndone y----> ", solids.length, " solids read");

        // write to BREP
        occ.writeBREP(filename + ".brep", solids, function done() {
        });
    } else {
        message = solids
        console.log("\n\n ERROR =", message);
    }

}, function progress(percent, incr) {
    pace.op(percent * 100 * 10)
    // console.log("tick ", incr,bar1.step)
    //  bar1.tick(incr);
});

console.log(process.argv[0]);
