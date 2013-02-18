var occ  = require('../../lib/occ'),
    CSGBuilder = require('../../lib/CSGBuilder'),
    shapeFactory = require('../../lib/shapeFactory'),
    scriptRunner = require('../../lib/scriptrunner');


var fs = require('fs');
var fileUtils = require ("file-utils");
var File = fileUtils.File;

var fast_occ = require('../../lib/fastbuilder').occ;

exports.buildCSG = function(req,res)
{
    "use strict";
    var jsonStr = "";

    var csgStr = CSGBuilder.sample_csg_tree;
    csgStr = req.body;

    try {
        var solid  = CSGBuilder.build(csgStr);
        var mesh   = solid.mesh;
        jsonStr = mesh.toJSON();
    }
    catch(err) {
        res.send(501,"Error building solid : "+ err.message + err.toString());
        return;
    }
    res.send(jsonStr);

};

var vm = require('vm');
var util = require('util');

function getBlockedFunctionConstructor() {
    "use strict";

    function FakeFunction(execCode) {
        var code = "FUNCTION_CONSTRUCTOR_DETECTED";
        var message = "function constructor with code: " + execCode;
        error = {code: code, message: message};

        return function() {};
    }

    return FakeFunction;
}

function buildResponse(solids,logs) {
    
    var response = { solids: [] , logs: [ ]};

    solids.forEach(function(solid){
        response.solids.push(occ.buildSolidMesh(solid));
    })
    response.logs = logs;
    return response;
}


exports.buildCSG1 = function(req,res)
{

    var csgFuncScript    = decodeURIComponent(req.body.script);
    var params       = req.body.params;
    var id           = req.body.id;

    var process = new scriptRunner.ScriptRunner({
        csg: fast_occ,
        occ: fast_occ,
        solids: [],
        display: function( objs) {
            if (!(objs instanceof occ.Solid)) {
                objs.forEach(function(o){ process.env.solids.push(o);});
            } else {
                process.env.solids.push(objs);
            }
        },
        shapeFactory: shapeFactory,
    });  

    var solidBuilderScript = ""+csgFuncScript+"";

    process.run(solidBuilderScript,
        function done_callback() {
            res.send(buildResponse(process.env.solids,process.env.logs));
            console.log(" All Done");
        },
        function error_callback(err) {
           res.send(501,"Error building solid : "+ err.message + "    "+ err.stack);       
        }
    );
};
