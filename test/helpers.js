const getTemporaryFilePath = require("gettemporaryfilepath");

const _getTemporaryFilePath = function () {
    let tmp = getTemporaryFilePath.apply(null, arguments);
    return tmp.replace(/\\/g, "/");
};
exports.getTemporaryFilePath = _getTemporaryFilePath;


const fs = require("fs");

function remove_file(filename, optional_callback) {

    fs.exists(filename, function (exists) {

        if (exists) {
            //Show in green
            //xx console.log('File exists. Deleting now ... ' + filename);
            fs.unlink(filename, optional_callback);
        } else {
            //Show in red
            console.log("File " + filename + " not found, so not deleting.");
            optional_callback || optional_callback();
        }
    });
}
exports.remove_file = remove_file;
