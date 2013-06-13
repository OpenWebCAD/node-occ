// test_STEP
var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");

var fs = require("fs");


var getTemporaryFilePath = require('gettemporaryfilepath');


describe("testing STEP input output ",function(){

  var b1_step, b2_step,b3_step;
  before(function() {
    b1_step = getTemporaryFilePath({ prefix: "b1_", suffix: ".step"});
    b2_step = getTemporaryFilePath({ prefix: "b2_", suffix: ".step"});
    b3_step = getTemporaryFilePath({ prefix: "b3_", suffix: ".step"});

  });
  after(function() {
    fs.unlink(b1_step);
    fs.unlink(b2_step);
    fs.unlink(b3_step);
  });

  it("should write a simple shape", function() {
     var box = occ.makeBox([0,0,0],[100,200,300]);
     occ.writeSTEP(b1_step,box);
     var cyl = occ.makeCylinder([0,0,0],[0,0,10],5);
     occ.writeSTEP(b2_step,cyl);
     occ.writeSTEP(b3_step,[box,cyl]);
  }); 

  describe(" readSTEP ",function() {
    it("should raise an exception with invalide arguments", function() {
      (function(){ occ.readSTEP(); }).should.throw();
      (function(){ occ.readSTEP("filename"); }).should.throw();
    });
    it("should call the callback with an error if the file doesn't exist", function(done) {
       occ.readSTEP("invalid file name",function(err,shapes) {
           console.log(" intercepting error ", err);
           assert(err !== undefined);
           done();
       });
    });
    it("should read the shape back",function(done) {
      
       occ.readSTEP(b1_step,function(err, shapes ) {
             
           shapes.length.should.equal(1);
           shapes[0].numFaces.should.equal(6);
           done();
       });
    });
    it("should read the shape back",function(done) {

       occ.readSTEP(b2_step,function(err, shapes ) {
           shapes.length.should.equal(1);
           shapes[0].numFaces.should.equal(3);
           done();
       });

    });
    it("should read the shape back",function(done) {
       occ.readSTEP(b3_step,function(err, shapes ) {
           shapes.length.should.equal(2);
           shapes[0].numFaces.should.equal(6);
           shapes[1].numFaces.should.equal(3);
           done();
       });
    });
  });
});
