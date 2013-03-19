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

function startChronometer() {
    return process.hrtime();  
}
function stopChronometer(time1) {
    var diff1 = process.hrtime(time1);
    diff1 = (diff1[0]*1E9+diff1[1]); // in nanoseconds
    diff1 /= 1000.0;                 // in microseconds
    diff1 /= 1000.0;                 // in miliseconds
    diff1 /= 1000.0;                 // in seconds
    return diff1;
}



describe("testing geometry builder",function(){

    before(function(){
        fastBuilder.resetCache();
    });
    it("should create a bottle faster the second time ", function() {

        fastBuilder.mapQueryCount.should.equal(0);
        fastBuilder.mapHit.should.equal(0);
        
        var c1 = startChronometer();
        makeShape();
        var diff1 = stopChronometer(c1);

        fastBuilder.mapQueryCount.should.equal(5);
        fastBuilder.mapHit.should.equal(0);

        var c2 = startChronometer();
        makeShape();
        var diff2 = stopChronometer(c2);


        fastBuilder.mapQueryCount.should.equal(10);
        fastBuilder.mapHit.should.equal(5);

        console.log(" time to compute first  box = ", diff1 ," seconds");
        console.log(" time to compute second box = ", diff2  ," seconds" );
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


describe("testing fast-builder with impossible cone" , function () {
    var solid1 = 0;
    before(function () {
        // this cone cannot be built : it has PI/2 for half-angle !         
    });
    it("should have no solid",function(){
        (function() {
            solid1 = fast_occ.makeCone( [0,0,0] , [0,0,1] , 1.5707963267948966 , 10);   
        }).should.throw();
    });
});

describe("testing fast-builder with LEGO brick" , function () {

    it("should produce a LEGO brick",function(){
        

        function buildBrick() {
            var factory = require("../lib/shapeFactory.js");

            var brick24 =  factory.makeLegoBrick(fast_occ,2,4,'thick');

            brick24.numFaces.should.be.greaterThan(40);

            // now check with bounding box
            var bbox = brick24.getBoundingBox();

            var eps = 0.01;
            bbox.nearPt.x.should.be.within(0-eps,0+eps);
            bbox.nearPt.y.should.be.within(0-eps,0+eps);
            bbox.nearPt.z.should.be.within(0-eps,0+eps);

            bbox.farPt.x.should.be.within(16-eps,16+eps);
            bbox.farPt.y.should.be.within(32-eps,32+eps);
            bbox.farPt.z.should.be.within(11.2-eps,11.2+eps);                   
        }

        var c1 = startChronometer();
        buildBrick();
        var diff1 = stopChronometer(c1);

        var c2 = startChronometer();
        buildBrick();
        var diff2 = stopChronometer(c2);

        console.log(" time to compute first  box = ", diff1 ," seconds" );
        console.log(" time to compute second box = ", diff2  ," seconds");
        var speedup = Math.round( (diff1-diff2)/diff2*100,2);
        console.log(" speed up                   = ", speedup ,"%" );

        diff1.should.be.greaterThan(diff2);
        speedup.should.be.greaterThan(200); //"%"

    }); 
});
