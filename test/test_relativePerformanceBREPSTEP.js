var occ = require('../lib/occ')

var ProgressBar  = require('progress')


function myReadStep(filname,done) {

  var bar = new ProgressBar("reading file [:bar] :percent elapsed :elapseds ETA :etas", {
        complete: '=' , 
        incomplete: '-' ,
        width: 100 , 
        total: 1000 
  });
var old = 0
function tick(percent)  {
   bar.tick(percent);
}

  occ.readSTEP('test/kuka.stp',function( err,solids) {
        var elapsed = new Date - bar.start;
        console.log("\ndone with " , solids.length , " solids  in ", Math.round(elapsed/100)/10 ," seconds" );

        var t1 = new Date
        occ.writeBREP("toto.brep",solids);
 	var t2 = new Date
	elapsed = t2 -t1
        console.log(" writing BREP file in " , Math.round(elapsed/10)/100 , " seconds")

        t1 = new Date
        occ.readBREP("toto.brep",function(err,solids) {
 	 	t2  =new Date
		elapsed = t2 -t1
        	console.log(" reading BREP file in " , Math.round(elapsed/10)/100 , " seconds")
                console.log(" nb solids = ", solids.length);
		done();
	});
  }, tick );
}

 //    myReadStep("test/kuka.stp");
describe("testing relative performance of BREP and STEP I/O",function()  {
  this.timeout(15000)
  it("should ...",function(done) {
     myReadStep("test/kuka.stp",done);
  } )
});

