
var occ = require("../lib/occ");
var should = require("should");

describe("testing various transformation",function() {


    function getVerticeData(box) {
        var vert = box.getVertices();
        vert = vert.map(function(v) { return [v.x, v.y, v.z]; });
        vert.length.should.eql(8);
        return vert;
    }
    function add(v1,v2) {
        return [v1[0]+v2[0],v1[1]+v2[1],v1[2]+v2[2]];
    }
    it("#applyTransform",function(){

        var box = occ.makeBox([0,0,0],[100,200,300]);

        var  trsf=new occ.Transformation();
        trsf.makeTranslation([10,20,30]);

        var vert = getVerticeData(box);

        vert[1].should.eql([0,0,0]);
        box.applyTransform(trsf);

        var vert_after = getVerticeData(box);

        // translate vertex
        var vert = vert.map(function(v){ return add(v,[10,20,30])});
        vert_after.should.eql(vert);


    });

});