import assert from "assert";
import should from "should";
import { makeLegoBrick, ISolid, occ } from "..";

import { removeFile, getTemporaryFilePath } from "./helpers";

describe("testing BREP input output ", function () {
    let b1_brep: string;
    let b2_brep: string;
    let b3_brep: string;
    let b1_volume = 0;
    let b1_area = 0;
    before(() => {
        b1_brep = getTemporaryFilePath({ prefix: "b1_", suffix: ".brep" });
        b2_brep = getTemporaryFilePath({ prefix: "b2_", suffix: ".brep" });
        b3_brep = getTemporaryFilePath({ prefix: "b3_", suffix: ".brep" });

        create_shapes();
    });
    after((done) => {
        removeFile(b1_brep);
        removeFile(b2_brep);
        removeFile(b3_brep);
        done();
    });

    function create_shapes() {
        let box = occ.makeBox([0, 0, 0], [100, 200, 300]);
        let b1_result = occ.writeBREP(b1_brep, box);
        b1_volume = box.volume;
        b1_area = box.area;

        let cyl = occ.makeCylinder([0, 0, 0], [0, 0, 10], 5);
        let b2_result = occ.writeBREP(b2_brep, cyl);

        let b3_result = occ.writeBREP(b3_brep, [box, cyl]);

        b1_result.should.eql(true);
        b2_result.should.eql(true);
        b3_result.should.eql(true);
    }

    it("should write a simple shape", function () {
        create_shapes();
    });

    describe(" readBREP ", function () {
        it("ZZ1 - should throw an error  if used with no argument", function () {
            should(function () {
                (occ.readBREP as any)(null, (err: Error) => {
                    err!.message.should.match(/expecting a filename/);
                });
            }).throwError();
        });

        it("ZZ2 - should call the callback method with an error if used with an invalid arguments", (done) => {
            occ.readBREP(
                "||this is a invalid filename||",
                (err?: Error | null, _shapes?: ISolid[]) => {
                    err!.message.should.match(/cannot read/);
                    done();
                }
            );
        });

        it("ZZ3 - should call the callback with an error if the file doesn't exist", (done) => {
            occ.readBREP(
                "invalid file name",
                (err?: Error | null, _shapes?: ISolid[]) => {
                    console.log(" intercepting error ", err);
                    assert(err !== undefined);
                    done();
                }
            );
        });

        it("ZZ4 - should read the shape back", (done) => {
            occ.readBREP(b1_brep, (err?: Error | null, shapes?: ISolid[]) => {
                should(err).eql(null);

                if (!err && shapes) {
                    shapes.length.should.equal(1);
                    shapes[0].numFaces.should.equal(6);

                    shapes[0].volume.should.equal(b1_volume);
                    shapes[0].area.should.equal(b1_area);
                }
                done(err);
            });
        });

        it("ZZ5 - should read the shape back", (done) => {
            occ.readBREP(b2_brep, (err?: Error | null, shapes?: ISolid[]) => {
                if (!err && shapes) {
                    shapes.length.should.equal(1);
                    shapes[0].numFaces.should.equal(3);
                }
                done(err);
            });
        });
        it("ZZ6 - should read the shape back", (done) => {
            occ.readBREP(b3_brep, (err?: Error | null, shapes?: ISolid[]) => {
                if (!err && shapes) {
                    shapes.length.should.equal(2);
                    shapes[0].numFaces.should.equal(6);
                    shapes[1].numFaces.should.equal(3);
                }
                done();
            });
        });
    });
});

function build_large_part() {
    let lego_filename = getTemporaryFilePath({
        prefix: "legoPlate3x2_2x2",
        suffix: "",
    });

    let legoPlate = makeLegoBrick(occ, 3, 2, "thin");
    let solids: ISolid[] = [];
    for (let x = 0; x < 100; x += 50) {
        for (let y = 0; y < 100; y += 50) {
            solids.push(legoPlate.translate([x, y, 0]));
        }
    }
    occ.writeBREP(lego_filename + ".brep", solids);

    /*
       occ.writeSTL(lego_filename + ".stl", solids);
  
       let obj = {solids: []};
       let counter = 0;
       solids.forEach(function (solid) {
       solid.name = "S" + counter;
       counter++;
       obj.solids.push(occ.buildSolidMesh(solid));
       });
       fs.writeFile(lego_filename + ".3js", JSON.stringify(obj, null, ""), function (err) {
       console.log("OK");
       });
  
       */
    return lego_filename;
}

describe("it should write and read a large brep file", function (this: Mocha.Suite) {
    this.timeout(15000);

    let filename = build_large_part();

    it("should read a large BREP file quickly", (done) => {
        console.log(" lego file ", filename);

        occ.readBREP(
            filename + ".brep",
            (err?: Error | null, solids?: ISolid[]) => {
                console.log(" read !!!");

                if (!err && solids) {
                    console.log(" num Faces = ", solids[0].numFaces);
                }
                done(err);
            }
        );
    });
});
