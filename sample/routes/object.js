var occ  = require('../../lib/occ'),
    CSGBuilder = require('../../lib/CSGBuilder'),
    shapeFactory = require('../../lib/shapeFactory'),
    jailguard = require('jailguard');


var fs = require('fs');
var fileUtils = require ("file-utils");
var File = fileUtils.File;

var fast_occ = require('../../lib/fastbuilder').occ;
var SecurityManager = fileUtils.SecurityManager;




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
    try {
        var  solidBuilderScript = decodeURIComponent(req.body.script);
        //xx console.log( "csg" , solidBuilderScript);

        var env = {
            csg: fast_occ,
            occ: fast_occ,
            logs: [],
            solids: [],
            print: function() { env.console.log.apply(env.console,arguments); },
            shapeFactory: shapeFactory,

                'console':    { 
                    log: function() { 
    			           console.log.apply(console,arguments);
    			           env.logs.push(arguments);
			            }  
                },
            'eval':        function() { throw "eval is forbidden";        },
            'require':     function() { throw "require is forbidden";     },
            'setTimeout':  function() { throw "setTimeout is forbidden";  },
            'setInterval': function() { throw "setInterval is forbidden"; },
            'display':    function(obj) {
                   env.solids.push(obj);
            },
            error: null } ;
       

        var code = solidBuilderScript;

        var settings = {
            prefix: "foo",
            suffix: ".bar",
            directory: "/temp/"
        };

        File.createTempFile (settings, function (error, file){
            if (!error) {
                var filename = file.toString ();
                console.log (" temporary file =",file.toString ()); //Prints: foo<random number>bar
                fs.writeFile(filename,code,function(err){

                    try {

                        vm.runInNewContext(code,env,filename);

                        res.send(buildResponse(env.solids,env.logs));

                    }
                    catch(err) {

                        console.trace("Here I am!");

                        function getLineNumber() {

                            var original = Error.prepareStackTrace;

                            var error = {};

                            Error.captureStackTrace(error, getLineNumber);

                            var lineNumber = error.stack;

                            Error.prepareStackTrace = original;

                            return lineNumber ;

                        }

                        console.log("transaction ended with an error",err.message);
                        console.log("error string = ", err.toString() );
                        console.log("error stack  = ", err.stack);

                        res.send(501,"Error building solid : "+ err.message + "    "+ err.stack);

                    }
                });


            }
        });

    }
    catch(err) {
        console.log(err.message + err.toString());
        res.send(501,"Error building solid : "+ err.message + "    "+ err.toString());
        return;
    }

}
