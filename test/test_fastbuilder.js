var assert = require("assert");

var fastBuilder = require("../lib/fastbuilder").fastBuilder;


function makeShape() {
    var e= 20;
    var s1 = fastBuilder.makeBox([10,e,30],[110,120,130]);
    var s2 = fastBuilder.makeBox(100,200,300);
    var s3 = fastBuilder.fuse(s1,s2);
    s3 =  s3.translate([0,20,30]);
    s3=  s3.translate([0,20,30]);
}


describe("testing geometry builder",function(){

    before(function(){

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


