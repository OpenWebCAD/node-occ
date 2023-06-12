
import path from "path";
import fs from "fs";
import "should";
import { promisify } from "util";
import { ISolid, occ } from "..";
import { removeFile, getTemporaryFilePath } from "./helpers";

const ProgressBar = require("progress");

async function myReadStep(filename: string): Promise<void> {

    let brepFilename = getTemporaryFilePath({ suffix: ".brep" });

    console.log("brepFilename", brepFilename);

    let bar = new ProgressBar("reading file [:bar] :percent elapsed :elapseds ETA :etas", {
        complete: "=",
        incomplete: "-",
        width: 100,
        total: 1000
    });

    let solids: ISolid[] = [];

    function progressFunc(percent: number) {
        bar.tick(percent);
    }

    function performMesh(solids: ISolid[]) {
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
                occ.buildSolidMesh(solid);
                let mesh = solid.mesh;
                solid.mesh.numVertices.should.be.greaterThan(3);
            }
            catch (err) {

            }
        }
        console.log("\n");
    }


    function chrono(async_func: (callback: (err?: Error | null) => void) => void, message: string, callback: (err?: Error | null) => void) {

        let t1: Date;
        let t2: Date;
        let elapsed: number;
        t1 = new Date();

        async_func((err?: Error | null) => {

            t2 = new Date();
            elapsed = Math.round((t2.getTime() - t1.getTime()) / 10) / 100;

            console.log("\ndone " + message + " in ", elapsed, " seconds");
            callback(err);
        });
    }


    async function read_original_step_file(): Promise<ISolid[]> {
        let t1 = new Date();
        return await promisify(occ.readSTEP)(filename);
    }

    async function perform_mesh_on_solids() {
        // make sure we have a mesh, so it can be saved in the BREP file
        performMesh(solids);
    }

    async function write_solids_to_brep() {
        occ.writeBREP(brepFilename, solids);
    }

    async function read_brep_file_again() {
        return await promisify(occ.readSTEP)(filename);
    }

    await read_original_step_file();
    await perform_mesh_on_solids();
    await write_solids_to_brep();
    await read_brep_file_again();
    await perform_mesh_on_solids();
    removeFile(brepFilename);


}

//    myReadStep("test/kuka.stp");
describe("testing relative performance of BREP and STEP I/O", function () {
    this.timeout(40000);
    it("should read kuka robot", async () => {
        let filename = path.join(__dirname, "kuka.stp");
        fs.existsSync(filename).should.eql(true, "kuka.stp should exists");
        await myReadStep(filename);
    });
});
