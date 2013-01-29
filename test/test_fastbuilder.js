var assert = require("assert");

var fastBuilder = require("../lib/fastbuilder").fastBuilder;


function makeShape() {
    var e= 20;
    var s1 = fastBuilder.makeBox([10,e,30],[110,120,130]);
    var s2 = fastBuilder.makeBox(100,200,300);
    var s3 = fastBuilder.fuse(s1,s2);
    s3 =  s3.translate([0,20,30]);
    s3=  s3.translate([0,20,30]);
    return s3;
}


describe("testing geometry builder",function(){

    before(function(){
        fastBuilder.resetCache();
    });
    it("should create a bottle faster the second time ", function() {

        fastBuilder.mapQueryCount.should.equal(0);
        fastBuilder.mapHit.should.equal(0);
        makeShape();
        fastBuilder.mapQueryCount.should.equal(5);
        fastBuilder.mapHit.should.equal(0);
        makeShape();
        fastBuilder.mapQueryCount.should.equal(10);
        fastBuilder.mapHit.should.equal(5);

    });
});

describe("testing calculateOperationHash",function(){
    'use strict';
    var calculateOperationHash = function() {
        return require("../lib/fastbuilder").calculateOperationHash("myFunc",arguments);
    }

    before(function(){

    });
    it("",function(){
         calculateOperationHash([10,20,30])[1].should.equal("myFunc([10,20,30])");
    })
});
describe("testing fast builder with array of shape",function(){

    before(function(){
        fastBuilder.resetCache();
    });
    it("should create a bottle faster the second time ", function() {

        fastBuilder.mapQueryCount.should.equal(0);
        fastBuilder.mapHit.should.equal(0);
        var a =[];
        a.push(makeShape());
        a.push(makeShape().translate(10,20,30));
        a.push(makeShape().translate(30,20,30));

        var compound = fastBuilder.compound(a);

    });
});

describe("testing fast builder with makeThickSolid" , function() {
    var s1;
    var s2;
    before(function(){
        s1 = fastBuilder.makeBox([10,20,30],[110,120,130]);
        s1 = fastBuilder.makeThickSolid(s1,s1.faces.top,6);

        var occ = require("../lib/occ");
        s2  = occ.makeBox([10,20,30],[110,120,130]);
        s2  = occ.makeThickSolid(s2,s2.faces.top,6);

    });
    it(" should construct the same object as if using 'occ' ",function(){
        s1.numFaces.should.equal(s2.numFaces);
    })
});

describe("testing fast builder with makeBottle" , function() {
    var s1;
    var s2;
    before(function(){
        s1 = fastBuilder.makeBottle();


    });
    it(" should ...",function(){
        s1.numFaces.should.be.greaterThan(16);
    })    
});
