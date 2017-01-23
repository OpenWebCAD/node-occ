const assert = require("assert");
const occ = require("../lib/occ");
const shapeFactory = require('../lib/shapeFactory');



describe("testing bounding box",function(){
    describe("an empty BoundingBox", function() {
        let bbox;
        before(function() {
             bbox = new occ.BoundingBox();
        });
        it("should  be void", function() {
            bbox.isVoid.should.equal(true);
        });
    });
    describe("an  BoundingBox built with a single point in constructor", function() {
        let bbox;
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
    describe("adding a single point to an empty bounding box", function(){
        let bbox;
        before(function() {
            bbox = new occ.BoundingBox();
            bbox.addPoint([10,20,30]);
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
    describe("checking calling isOut on a empty box",function() {
        it("should return isOut = true for any point ",function(){
            let bbox= new occ.BoundingBox();
            bbox.isOut([10,20,30]).should.equal(true);
        });
    });
    describe("checking calling isOut this box [-10,-10,-10],[5,5,5]",function() {
        let bbox= new occ.BoundingBox([-10,-10,-10],[5,5,5]);
        it("should return isOut = true for [10,20,30] ",function(){
            bbox.isOut([10,20,30]).should.equal(true);
        });
        it("should return isOut = false for [1,2,3] ",function(){
            bbox.isOut([1,2,3]).should.equal(false);
        });
    });


});
