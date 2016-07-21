var getTemporaryFilePath = require('gettemporaryfilepath');

var _getTemporaryFilePath = function () {
    var tmp = getTemporaryFilePath.apply(null, arguments);
    return tmp.replace(/\\/g, "/");
};
exports.getTemporaryFilePath = _getTemporaryFilePath;


var fs = require("fs");

function remove_file(filename) {

    fs.exists(filename, function (exists) {

        if (exists) {
            //Show in green
            //xx console.log('File exists. Deleting now ... ' + filename);
            fs.unlink(filename);
        } else {
            //Show in red
            console.log("File " + filename + " not found, so not deleting.");
        }
    });
}
exports.remove_file = remove_file;
