var assert = require("assert");
var should = require("should");

var occ = require("../lib/occ");


describe("testing BREP input output ",function(){

  it("should write a simple shape", function() {
     var box = occ.makeBox([0,0,0],[100,200,300]);
     occ.writeBREP("/temp/b1.brep",box);
     var cyl = occ.makeCylinder([0,0,0],[0,0,10],5);
     occ.writeBREP("/temp/b2.brep",cyl);
     occ.writeBREP("/temp/b3.brep",[box,cyl]);
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
      
       occ.readBREP("/temp/b1.brep",function(err, shapes ) {
             
           shapes.length.should.equal(1);
           shapes[0].numFaces.should.equal(6);
           done();
       });
    });
    it("should read the shape back",function(done) {

       occ.readBREP("/temp/b2.brep",function(err, shapes ) {
           shapes.length.should.equal(1);
           shapes[0].numFaces.should.equal(3);
           done();
       });

    });
    it("should read the shape back",function(done) {
       occ.readBREP("/temp/b3.brep",function(err, shapes ) {
           shapes.length.should.equal(2);
           shapes[0].numFaces.should.equal(6);
           shapes[1].numFaces.should.equal(3);
           done();
       });
    });
  });
});
