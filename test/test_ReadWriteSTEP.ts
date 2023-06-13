/*eslint-env node mocha*/
/*global require*/
// test_STEP
import assert from "assert";
import "should";
import { occ } from "..";
import { getTemporaryFilePath, removeFile } from "./helpers";

describe("testing STEP input output ", function () {
  let b1_step: string;
  let b2_step: string;
  let b3_step: string;

  before(function () {
    b1_step = getTemporaryFilePath({ prefix: "b1_", suffix: ".step" });
    b2_step = getTemporaryFilePath({ prefix: "b2_", suffix: ".step" });
    b3_step = getTemporaryFilePath({ prefix: "b3_", suffix: ".step" });

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
    removeFile(b1_step);
    removeFile(b2_step);
    removeFile(b3_step);
  });

  it("AZ0 - should write a simple shape", function (done) {
    let box = occ.makeBox([0, 0, 0], [100, 200, 300]);
    let b1 = occ.writeSTEP(b1_step, box);
    done();
  });

  it("AZ1 - readSTEP with callback ", function (done) {
    occ.readSTEP(b3_step, (err, shapes) => {
      console.log(err, shapes);
      shapes.length.should.equal(2);
      shapes[0].numFaces.should.equal(6);
      shapes[1].numFaces.should.equal(3);
      done();
    });
  });

  it("AZ2 - should raise an exception with invalid arguments", function () {
    (function () {
      (occ as any).readSTEP();
    }).should.throwError();

    (function () {
      (occ as any).readSTEP("filename");
    }).should.throwError();
  });

  it("AZ3 - should call the callback with an error if the file doesn't exist", function (done) {
    occ.readSTEP("invalid file name", function (err, shapes) {
      if (err) {
        err.message.should.match(/invalid file name/);
      } else {
        return done(new Error("Expecting Error"));
      }
      done();
    });
  });
  it("AZ4 - should read file one", function (done) {
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

  it("AZ5 - should read file two", function (done) {
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
  it("AZ6 - should read file three", function (done) {
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
