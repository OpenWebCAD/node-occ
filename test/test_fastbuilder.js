var assert = require("assert");

var fastBuilder = require("../lib/fastbuilder");

var fast_occ = fastBuilder.occ;
fastBuilder  = fastBuilder.fastBuilder; 

function makeShape() {
    var e= 20;
    var s1 = fast_occ.makeBox([10,e,30],[110,120,130]);
    var s2 = fast_occ.makeBox(100,200,300);
    var s3 = fast_occ.fuse(s1,s2);
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
        
        var time1 = process.hrtime();
        makeShape();
        var diff1 = process.hrtime(time1);
        diff1 = diff1[0]*1E9+diff1[1];

        fastBuilder.mapQueryCount.should.equal(5);
        fastBuilder.mapHit.should.equal(0);

        var time2 = process.hrtime();
        makeShape();
        var diff2 = process.hrtime(time2);
        diff2 = diff2[0]*1E9+diff2[1];

        fastBuilder.mapQueryCount.should.equal(10);
        fastBuilder.mapHit.should.equal(5);

        console.log(" time to compute first  box = ", diff1 );
        console.log(" time to compute second box = ", diff2 );
        console.log(" speed up                   = ", Math.round( (diff1-diff2)/diff2*100,2) ,"%" );

        diff1.should.be.greaterThan(diff2);

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

        var compound = fast_occ.compound(a);

    });
});

describe("testing fast builder with makeThickSolid" , function() {
    var s1;
    var s2;
    before(function(){
        s1 = fast_occ.makeBox([10,20,30],[110,120,130]);
        s1 = fast_occ.makeThickSolid(s1,s1.faces.top,6);

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
       // s1 = fast_occ.makeBottle();


    });
    it(" should ...",function(){
      //  s1.numFaces.should.be.greaterThan(16);
    })    
});



describe("testing fast builder get Common Edge" , function() {
  var solid1;
  var solid2;
  function buildFilletOnTopLeftEdge()
  {    
     var s1 = fast_occ.makeBox([10,20,30],[110,120,130]);
     var edges = s1.getCommonEdges(s1.faces.front,s1.faces.left);
     s1 = fast_occ.makeFillet(s1,edges,10);
     console.log(Object.keys(s1.faces));
     s1 = fast_occ.makeDraftAngle(s1,s1.faces["mleft:0"],0.1,s1.faces["mbottom:0"]);
     return s1;
  }
  before(function(){
    solid1 = buildFilletOnTopLeftEdge();
    solid2 = buildFilletOnTopLeftEdge();
  });
  it("should have 7 faces",function(){
    solid1.numFaces.should.be.equal(7);
    solid2.numFaces.should.be.equal(7);
  });
});
