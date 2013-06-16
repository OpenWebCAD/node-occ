var occ = require('../lib/occ');
var should = require("should");

var ProgressBar  = require('progress');


function myReadStep(filname,done) {

  var bar = new ProgressBar("reading file [:bar] :percent elapsed :elapseds ETA :etas", {
        complete: '=' , 
        incomplete: '-' ,
        width: 100 , 
        total: 1000 
  });
var old = 0;
function tick(percent)  {
   bar.tick(percent);
}

function performMesh(solids)
{
  var bar = new ProgressBar("meshing solids [:bar] :percent elapsed :elapseds ETA :etas", {
        complete: '=' , 
        incomplete: '-' ,
        width: 100 , 
        total: solids.length 
  });
  for ( var i  in solids ) {
    bar.tick();
    var solid =  solids[i];
    solid.numFaces.should.be.greaterThan(1);
    var mesh = solid.mesh;
    solid.mesh.numVertices.should.be.greaterThan(3);
  }
  console.log("\n");
}

  occ.readSTEP('test/kuka.stp',function( err,solids) {
        var t1,t2,elapsed;
        t1 = bar.start;
        t2 = new Date();
        elapsed = Math.round((t2 -t1)/10)/100;
        console.log("\ndone with " , solids.length , " solids  in ", elapsed ," seconds" );
 
        // make sure we have a mesh, so it can be saved in the BREP file 
        t1 = new Date();
        performMesh(solids);
        t2 = new Date();
        elapsed = Math.round((t2 -t1)/10)/100;
        console.log(" meshing solids  in " , elapsed , " seconds");


        t1 = new Date();
        occ.writeBREP("toto.brep",solids);
        t2 = new Date();
        elapsed = Math.round((t2 -t1)/10)/100;
        console.log(" writing BREP file in " , elapsed , " seconds");

        t1 = new Date();
        occ.readBREP("toto.brep",function(err,solids) {
        t2  =new Date();
        elapsed = Math.round((t2 -t1)/10)/100;
        console.log(" reading BREP file in " ,elapsed , " seconds");
        console.log(" nb solids = ", solids.length);
        t1 = new Date();
        performMesh(solids);
        t2 = new Date();
        elapsed = Math.round((t2 -t1)/10)/100;
        console.log(" meshing solids  in " , elapsed , " seconds");
        done();
  });
  }, tick );
}

 //    myReadStep("test/kuka.stp");
describe("testing relative performance of BREP and STEP I/O",function()  {
  this.timeout(15000);
  it("should ...",function(done) {
     myReadStep("test/kuka.stp",done);
  });
});
