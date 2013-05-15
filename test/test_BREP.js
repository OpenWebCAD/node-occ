var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");

var fs = require("fs");


var getTemporaryFilePath = require('gettemporaryfilepath');


describe("testing BREP input output ",function(){

  var b1_brep, b2_brep,b3_brep;
  before(function() {
    b1_brep = getTemporaryFilePath({ prefix: "b1_", suffix: ".brep"});
    b2_brep = getTemporaryFilePath({ prefix: "b2_", suffix: ".brep"});
    b3_brep = getTemporaryFilePath({ prefix: "b3_", suffix: ".brep"});

  });
  after(function() {
    fs.unlink(b1_brep);
    fs.unlink(b2_brep);
    fs.unlink(b3_brep);
  });

  it("should write a simple shape", function() {
     var box = occ.makeBox([0,0,0],[100,200,300]);
     occ.writeBREP(b1_brep,box);
     var cyl = occ.makeCylinder([0,0,0],[0,0,10],5);
     occ.writeBREP(b2_brep,cyl);
     occ.writeBREP(b3_brep,[box,cyl]);
  }); 

  describe(" readBREP ",function() {
    it("should raise an exception with invalide arguments", function() {
      (function(){ occ.readBREP(); }).should.throw();
      (function(){ occ.readBREP("filename"); }).should.throw();
    });
    it("should call the callback with an error if the file doesn't exist", function(done) {
       occ.readBREP("invalid file name",function(err,shapes) {
           console.log(" intercepting error ", err);
           assert(err !== undefined);
           done();
       });
    });
    it("should read the shape back",function(done) {
      
       occ.readBREP(b1_brep,function(err, shapes ) {
             
           shapes.length.should.equal(1);
           shapes[0].numFaces.should.equal(6);
           done();
       });
    });
    it("should read the shape back",function(done) {

       occ.readBREP(b2_brep,function(err, shapes ) {
           shapes.length.should.equal(1);
           shapes[0].numFaces.should.equal(3);
           done();
       });

    });
    it("should read the shape back",function(done) {
       occ.readBREP(b3_brep,function(err, shapes ) {
           shapes.length.should.equal(2);
           shapes[0].numFaces.should.equal(6);
           shapes[1].numFaces.should.equal(3);
           done();
       });
    });
  });
});
