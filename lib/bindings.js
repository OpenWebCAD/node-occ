const binary = require("node-pre-gyp");
const path = require("path");
const bindingPath = binary.find(path.resolve(path.join(__dirname, "../package.json")));
const binding = require(bindingPath);
module.exports = binding;// was require("../build/Release/occ");
//xx module.exports = require("../build/Debug/occ");