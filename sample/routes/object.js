"use strict";
var occ  = require('../../lib/occ'),
    shapeFactory = require('../../lib/shapeFactory'),
    scriptRunner = require('../../lib/scriptrunner');


var fs = require('fs');
var path = require("path");
var fileUtils = require ("file-utils");

var fast_occ = require('../../lib/fastbuilder').occ;


var vm = require('vm');
var util = require('util');


function buildResponse(solids,logs) {
    
    var response = { solids: [] , logs: [ ]};
    var counter = 1;
    solids.forEach(function(solid){
        solid.name = "S" + counter;
        counter++;
        response.solids.push(occ.buildSolidMesh(solid));
    });
    response.logs = logs;
    return response;
}


exports.buildCSG1 = function(req,res)
{

    console.log("req",req);

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
            res.status(501).send("Error building solid : " + err.message + "    " + err.stack);
        }
    );
};


exports.load_cadfile = function(req,res) {


    var filename = path.join(__dirname, "..", req.body.filename);

    function progress(percent) {
       console.log(" -------------------> ", percent);
    }

    console.log(" loading ", filename);

    occ.readSTEP(filename, function (err, solids) {
            if (err) {
                console.log(" readStep has failed", err.message);
                res.status(501).send("Error building solid : " + err.message + "    " + err.stack);
            } else {
                console.log(" readStep has succeeded");
                res.send(buildResponse(solids,[]));
            }
    },progress);



};
