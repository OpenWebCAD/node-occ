"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const async = __importStar(require("async"));
const path_1 = __importDefault(require("path"));
require("should");
const __1 = require("..");
const ProgressBar = require("progress");
const remove_file = require("./helpers").remove_file;
function myReadStep(filename, done) {
    let brep_filename = require("./helpers").getTemporaryFilePath({ suffix: ".brep" });
    let bar = new ProgressBar("reading file [:bar] :percent elapsed :elapseds ETA :etas", {
        complete: "=",
        incomplete: "-",
        width: 100,
        total: 1000
    });
    let solids = [];
    function progressFunc(percent) {
        bar.tick(percent);
    }
    function performMesh(solids) {
        let bar = new ProgressBar("meshing solids [:bar] :percent elapsed :elapseds  ETA :etas", {
            complete: "=",
            incomplete: "-",
            width: 100,
            total: solids.length
        });
        for (let i in solids) {
            bar.tick();
            let solid = solids[i];
            solid.numFaces.should.be.greaterThan(1);
            solid.name = "solid_" + i;
            try {
                __1.occ.buildSolidMesh(solid);
                let mesh = solid.mesh;
                solid.mesh.numVertices.should.be.greaterThan(3);
            }
            catch (err) {
            }
        }
        console.log("\n");
    }
    function chrono(async_func, message, callback) {
        let t1;
        let t2;
        let elapsed;
        t1 = new Date();
        async_func((err) => {
            t2 = new Date();
            elapsed = Math.round((t2.getTime() - t1.getTime()) / 10) / 100;
            console.log("\ndone " + message + " in ", elapsed, " seconds");
            callback(err);
        });
    }
    function read_original_step_file(callback) {
        let t1 = new Date();
        __1.occ.readSTEP(filename, (err, _solids) => {
            let solids = _solids;
            if (err) {
                return callback(new Error(" readStep returned error = " + err.message + " while reading " + filename));
            }
            else {
                console.log(" read ", solids.length, " solids");
            }
            callback(err);
        });
    }
    function perform_mesh_on_solids(callback) {
        // make sure we have a mesh, so it can be saved in the BREP file
        performMesh(solids);
        callback();
    }
    function write_solids_to_brep(callback) {
        __1.occ.writeBREP(brep_filename, solids);
        callback();
    }
    function read_brep_file_again(callback) {
        __1.occ.readBREP(brep_filename, (err, _solids) => {
            if (!err) {
                let solids = _solids;
                console.log(" nb solids = ", solids.length);
            }
            return callback(err);
        });
    }
    async.series([
        chrono.bind(null, read_original_step_file, "read_original_step_file"),
        chrono.bind(null, perform_mesh_on_solids, "perform_mesh_on_solids"),
        chrono.bind(null, write_solids_to_brep, "write_solids_to_brep"),
        chrono.bind(null, read_brep_file_again, "read_brep_file_again"),
        chrono.bind(null, perform_mesh_on_solids, "perform_mesh_on_solids"),
        (err) => {
            remove_file(brep_filename, err);
        }
    ], done);
}
//    myReadStep("test/kuka.stp");
describe("testing relative performance of BREP and STEP I/O", function () {
    this.timeout(40000);
    it("should read kuka robot", (done) => {
        let filename = path_1.default.join(__dirname, "kuka.stp");
        myReadStep(filename, done);
    });
});
