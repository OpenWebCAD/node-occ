const assert = require("assert");
const should = require("should");
const shape_factory = require("../lib/shapeFactory");
const occ = require("../lib/occ");
const fs = require("fs");


const getTemporaryFilePath = require("./helpers").getTemporaryFilePath;
const remove_file = require("./helpers").remove_file;


describe("testing BREP input output ", function () {

    let b1_brep, b2_brep, b3_brep;
    let b1_volume = 0;
    let b1_area = 0;
    before(function () {

        b1_brep = getTemporaryFilePath({prefix: "b1_", suffix: ".brep"});
        b2_brep = getTemporaryFilePath({prefix: "b2_", suffix: ".brep"});
        b3_brep = getTemporaryFilePath({prefix: "b3_", suffix: ".brep"});

        create_shapes();

    });
    after(function (done) {
        remove_file(b1_brep);
        remove_file(b2_brep);
        remove_file(b3_brep);
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

        it("ZZ - should throw an error  if used with no argument", function () {

            should(function () {
                occ.readBREP(null, function (err, shapes) {
                    err.message.should.match(/expecting a filename/);
                });
            }).throwError();
        });

        it("ZZ - should call the callback method with an error if used with an invalid arguments", function (done) {

            occ.readBREP("||this is a invalid filename||", function (err, shapes) {
                err.message.should.match(/cannot read/);
                done();
            });
        });

        it("should call the callback with an error if the file doesn't exist", function (done) {
            occ.readBREP("invalid file name", function (err, shapes) {
                console.log(" intercepting error ", err);
                assert(err !== undefined);
                done();
            });
        });

        it("should read the shape back", function (done) {

            occ.readBREP(b1_brep, function (err, shapes) {

                should(err).eql(null);

                if (!err) {
                    shapes.length.should.equal(1);
                    shapes[0].numFaces.should.equal(6);

                    shapes[0].volume.should.equal(b1_volume);
                    shapes[0].area.should.equal(b1_area);
                }
                done(err);
            });
        });

        it("should read the shape back", function (done) {

            occ.readBREP(b2_brep, function (err, shapes) {
                if (!err) {
                    shapes.length.should.equal(1);
                    shapes[0].numFaces.should.equal(3);
                }
                done(err);
            });

        });
        it("should read the shape back", function (done) {
            occ.readBREP(b3_brep, function (err, shapes) {
                if (!err) {
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

    let lego_filename = getTemporaryFilePath({prefix: "legoPlate3x2_2x2", suffix: ""});

    let legoPlate = shape_factory.makeLegoBrick(occ, 3, 2, "thin");
    let solids = [];
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

describe("it should write and read a large brep file", function () {

    this.timeout(15000);

    let filename = build_large_part();

    it("should read a large BREP file quickly", function (done) {

        console.log(" lego file ", filename);

        occ.readBREP(filename + ".brep", function (err, solids) {

            console.log(" read !!!");

            if (!err) {
                console.log(" num Faces = ", solids[0].numFaces);
            }
            done(err);
        });

    });


});
