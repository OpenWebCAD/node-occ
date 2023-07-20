"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
/*eslint-env node mocha*/
/*global require*/
// test_STEP
const assert_1 = __importDefault(require("assert"));
require("should");
const __1 = require("..");
const helpers_1 = require("./helpers");
describe("testing STEP input output ", function () {
    let b1_step;
    let b2_step;
    let b3_step;
    before(function () {
        b1_step = (0, helpers_1.
            mporaryFilePath)({ prefix: "b1_", suffix: ".step" });
        b2_step = (0, helpers_1.getTemporaryFilePath)({ prefix: "b2_", suffix: ".step" });
        b3_step = (0, helpers_1.getTemporaryFilePath)({ prefix: "b3_", suffix: ".step" });
        let box = __1.occ.makeBox([0, 0, 0], [100, 200, 300]);
        let b1 = __1.occ.writeSTEP(b1_step, box);
        let cyl = __1.occ.makeCylinder([0, 0, 0], [0, 0, 10], 5);
        let b2 = __1.occ.writeSTEP(b2_step, cyl);
        let b3 = __1.occ.writeSTEP(b3_step, [box, cyl]);
        b1.should.eql(true);
        b2.should.eql(true);
        b3.should.eql(true);
    });
    after(function () {
        (0, helpers_1.removeFile)(b1_step);
        (0, helpers_1.removeFile)(b2_step);
        (0, helpers_1.removeFile)(b3_step);
    });
    it("AZ0 - should write a simple shape", function (done) {
        let box = __1.occ.makeBox([0, 0, 0], [100, 200, 300]);
        let b1 = __1.occ.writeSTEP(b1_step, box);
        done();
    });
    it("AZ1 - readSTEP with callback ", function (done) {
        __1.occ.readSTEP(b3_step, (err, shapes) => {
            console.log(err, shapes);
            shapes.length.should.equal(2);
            shapes[0].numFaces.should.equal(6);
            shapes[1].numFaces.should.equal(3);
            done();
        });
    });
    it("AZ2 - should raise an exception with invalid arguments", function () {
        (function () {
            __1.occ.readSTEP();
        }).should.throwError();
        (function () {
            __1.occ.readSTEP("filename");
        }).should.throwError();
    });
    it("AZ3 - should call the callback with an error if the file doesn't exist", function (done) {
        __1.occ.readSTEP("invalid file name", function (err, shapes) {
            if (err) {
                err.message.should.match(/invalid file name/);
            }
            else {
                return done(new Error("Expecting Error"));
            }
            done();
        });
    });
    it("AZ4 - should read file one", function (done) {
        __1.occ.readSTEP(b1_step, function (err, shapes) {
            if (err) {
                console.log(" err = ", err, shapes);
            }
            (0, assert_1.default)(!err);
            shapes.length.should.equal(1);
            shapes[0].numFaces.should.equal(6);
            done();
        });
    });
    it("AZ5 - should read file two", function (done) {
        __1.occ.readSTEP(b2_step, function (err, shapes) {
            if (err) {
                console.log(" err = ", err, shapes);
            }
            (0, assert_1.default)(!err);
            shapes.length.should.equal(1);
            shapes[0].numFaces.should.equal(3);
            done();
        });
    });
    it("AZ6 - should read file three", function (done) {
        __1.occ.readSTEP(b3_step, function (err, shapes) {
            if (err) {
                console.log(" err = ", err, shapes);
            }
            (0, assert_1.default)(!err);
            shapes.length.should.equal(2);
            shapes[0].numFaces.should.equal(6);
            shapes[1].numFaces.should.equal(3);
            done();
        });
    });
});
