var should = require('should');

var CSGTree = require("../lib/CSGTree.js");


function build_cooker_base(csg)
{
    var height = 20;
    var thickness = 2;
    var radius  =10.0;



    var s1 = csg.makeCylinder([0,0,0],[0,0,height] , radius);
    var s2 = csg.makeSphere([0,0,0], radius);
    var s3 = csg.makeCylinder([0,0,-radius*0.7],[0,0,height], radius*2);
    var s4 = csg.fuse(s1,s2);
    var s5 = csg.common(s4,s3);


    var r1 = csg.makeCylinder([0,0,thickness],[0,0,height] , radius-thickness);
    var r2 = csg.makeSphere([0,0,0],radius-thickness);
    var r3 = csg.makeCylinder( [0,0,-radius*0.7+thickness],[0,0,height], radius*2);
    var r4 = csg.fuse(r1,r2);
    var r5 = csg.common(r4,r3);

    var solid  = csg.cut(s5,r5);
    return solid;
}

function cooker_base()
{
    var  csg= new CSGTree.CSGTree();

    csg.root =  build_cooker_base(csg);
    return csg;
}

describe("Building a csg on the client side",function() {
    var csg;
    var compiled_csg;
    before(function(){
       csg =  cooker_base();
       compiled_csg = csg.toJSON();

    });
    it("should produce a parsable JSON string",function(){
        console.log(JSON.stringify(compiled_csg))
    });
    it("should have a root element",function(){
       compiled_csg.hasOwnProperty('root').should.be.true;
    });
    it("should have a operation matching root element",function(){
        compiled_csg.hasOwnProperty(compiled_csg["root"]).should.be.true;
    });
    it("should have 12 operations",function(){
        Object.keys(compiled_csg).length.should.equal(12);
    });
});


var CSGBuilder = require("../lib/CSGBuilder");
var occ = require("../lib/occ");

describe("Building a csg on the client and using occ shall produce the same result",function(){
    var csg;
    var compiled_csg;
    var solid1;
    var solid2;
    before(function(){

        // building a csg
        csg =  cooker_base();
        compiled_csg = csg.toJSON();

        // executing the csg using the CSGBuilderEngine
        console.log("----- building object with CSGBuilder");
        solid1 = CSGBuilder.build(compiled_csg);
        console.log("... done");
        // executing on occ
        console.log("----- building object directly with occ");
        solid2 = build_cooker_base(occ);
        console.log("... done");

    });
    it("it should produce the same geometry",function() {
        solid1.numFaces.should.equal(solid2.numFaces);
        solid1.mesh.numVertices.should.equal(solid2.mesh.numVertices);

    });
});


