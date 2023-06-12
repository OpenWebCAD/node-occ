"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.removeFile = exports.getTemporaryFilePath = void 0;
const node_fs_1 = __importDefault(require("node:fs"));
const node_os_1 = __importDefault(require("node:os"));
const node_crypto_1 = __importDefault(require("node:crypto"));
const node_path_1 = __importDefault(require("node:path"));
function getTemporaryFilePath({ prefix, suffix }) {
    const name = node_crypto_1.default.randomUUID();
    return node_path_1.default.join(node_os_1.default.tmpdir(), (prefix || "") + name + suffix);
}
exports.getTemporaryFilePath = getTemporaryFilePath;
;
function removeFile(filename) {
    if (node_fs_1.default.existsSync(filename)) {
        node_fs_1.default.unlinkSync(filename);
    }
    else {
        //Show in red
        console.log("File " + filename + " not found, so not deleting.");
    }
}
exports.removeFile = removeFile;
