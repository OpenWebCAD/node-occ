var assert = require("assert");

var occ = require("../lib/occ");

function MakeBottle()
{
    var myWidth     = 10.0;
    var myThickness =  2.0;

    var  aPnt1 = [ -myWidth / 2. ,                0  , 0];
    var  aPnt2 = [ -myWidth / 2. , -myThickness / 4. , 0];
    var  aPnt3 = [            0  , -myThickness / 2. , 0];
    var  aPnt4 = [  myWidth / 2. , -myThickness / 4. , 0];
    var  aPnt5 = [  myWidth / 2. ,                0  , 0];

    var segment1 = new occ.Edge().createLine(aPnt1,aPnt2);
    var arc1     = new occ.Edge().createArc3P(aPnt2,aPnt3,aPnt4);
    var segment2 = new occ.Edge().createLine(aPnt4,aPnt5);

    var wire  = new occ.Wire(segment1,arc1,segment2);

}



describe("testing geometry builder",function(){
	it("should create a bottle", function() {
        MakeBottle();
	});
});
