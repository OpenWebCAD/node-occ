var assert = require("assert");

var occ = require("../lib/occ");
var shapeFactory = require('../lib/shapeFactory');



describe("testing bounding box",function(){
    describe("an empty BoundingBox", function() {
        var bbox;
        before(function() {
             bbox = new occ.BoundingBox();
        });
        it("should  be void", function() {
            bbox.isVoid.should.equal(true);
        });
    });
    describe("an  BoundingBox built with a single point", function() {
        var bbox;
        before(function() {
            bbox = new occ.BoundingBox([10,20,30]);
        });
        it("should  not be void", function() {
            bbox.isVoid.should.equal(false);
        });
        it("should have nearPt to be correct", function() {
            bbox.nearPt.x.should.equal(10);
            bbox.nearPt.y.should.equal(20);
            bbox.nearPt.z.should.equal(30);
        });
        it("should have farPt to be correct", function() {
            bbox.farPt.x.should.equal(10);
            bbox.farPt.y.should.equal(20);
            bbox.farPt.z.should.equal(30);
        });
    });
});
