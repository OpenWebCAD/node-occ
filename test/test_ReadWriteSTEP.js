/*eslint-env node mocha*/
/*global require*/
// test_STEP
const assert = require("assert");
require("should");
const occ = require("../lib/occ");



const getTemporaryFilePath = require("./helpers").getTemporaryFilePath;
const remove_file = require("./helpers").remove_file;

describe("testing STEP input output ", function () {

    let b1_step, b2_step, b3_step;

    before(function () {

        b1_step = getTemporaryFilePath({prefix: "b1_", suffix: ".step"});
        b2_step = getTemporaryFilePath({prefix: "b2_", suffix: ".step"});
        b3_step = getTemporaryFilePath({prefix: "b3_", suffix: ".step"});

        let box = occ.makeBox([0, 0, 0], [100, 200, 300]);
        let b1 = occ.writeSTEP(b1_step, box);


        let cyl = occ.makeCylinder([0, 0, 0], [0, 0, 10], 5);
        let b2 = occ.writeSTEP(b2_step, cyl);
        let b3 = occ.writeSTEP(b3_step, [box, cyl]);

        b1.should.eql(true);
        b2.should.eql(true);
        b3.should.eql(true);
    });
    after(function () {
        remove_file(b1_step);
        remove_file(b2_step);
        remove_file(b3_step);
    });

    it("should write a simple shape", function (done) {
        let box = occ.makeBox([0, 0, 0], [100, 200, 300]);
        let b1 = occ.writeSTEP(b1_step, box);
        done();
    });

    it(" readSTEP with callback ", function (done) {

        let callback_called = 0;
        occ.readSTEP(b3_step, function (err, shapes) {
            shapes.length.should.equal(2);
            shapes[0].numFaces.should.equal(6);
            shapes[1].numFaces.should.equal(3);
            callback_called.should.be.greaterThan(-1);
            done();
        }, function callback(message, percent) {
            callback_called++;
        });
    });


    it("should raise an exception with invalid arguments", function () {
        (function () {
            occ.readSTEP();
        }).should.throwError();

        (function () {
            occ.readSTEP("filename");
        }).should.throwError();

    });

    it("should call the callback with an error if the file doesn't exist", function (done) {

        occ.readSTEP("invalid file name", function (err, shapes) {
            err.message.should.match(/invalid file name/);
            done();
        });
    });
    it("should read file one", function (done) {

        occ.readSTEP(b1_step, function (err, shapes) {
            if (err) {
                console.log(" err = ", err, shapes);
            }
            assert(!err);
            shapes.length.should.equal(1);
            shapes[0].numFaces.should.equal(6);
            done();
        });
    });

    it("should read file two", function (done) {

        occ.readSTEP(b2_step, function (err, shapes) {
            if (err) {
                console.log(" err = ", err, shapes);
            }
            assert(!err);
            shapes.length.should.equal(1);
            shapes[0].numFaces.should.equal(3);
            done();
        });

    });
    it("should read file three", function (done) {
        occ.readSTEP(b3_step, function (err, shapes) {
            if (err) {
                console.log(" err = ", err, shapes);
            }
            assert(!err);
            shapes.length.should.equal(2);
            shapes[0].numFaces.should.equal(6);
            shapes[1].numFaces.should.equal(3);
            done();
        });
    });

});
