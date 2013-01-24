var occ  = require('../../lib/occ'),
    CSGBuilder = require('../../lib/CSGBuilder'),
    shapeFactory = require('../../lib/shapeFactory'),
    jailguard = require('jailguard');

var fs = require('fs');
var fileUtils = require ("file-utils");
var File = fileUtils.File;

var fastocc = require('../../lib/fastbuilder').fastBuilder;

var SecurityManager = fileUtils.SecurityManager;

exports.get = function(req, res) {

    var obj = database.findObject(req.params.id);

    res.format({
        text: function(){
            res.send('hey : ' + JSON.stringify(obj,null," "));
        },

        html: function(){
            res.send('<p>hey</p>'  + JSON.stringify(obj,null," ") + " params = " +  req.params.id + " " +JSON.stringify(req.params) + " query = " +  JSON.stringify(req.query)  );
        },

        json: function(){
            res.send(obj);
        }
    });
  // res.send('user ' + req.params.id);
}
exports.list = function(req, res) {


    res.format({
        text: function(){
            res.send('hey : ' + JSON.stringify(database.solidMap,null," "));
        },

        html: function(){
            res.send('<p>hey</p>'  + JSON.stringify(database.solidMap,null," "));
        },

        json: function(){
            res.send(database.solidMap);
        }
    });
}


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

function buildMesh(solid)
{
    return buildFacesMesh(solid);

    var mesh   = solid.mesh ;
    jsonStr = mesh.toJSON();
    return jsonStr;
}

function buildFacesMesh(solid)
{
    // make sure object is mesh
    var mesh = solid.mesh;

    // produce each faces
    var faces = solid.getFaces();
    var face;
    var responseObject =[];
    for (var i=0;i<faces.length;i++) {
        face = faces[i];

        var r = Math.floor((Math.random()*250)+1);
        var g = Math.floor((Math.random()*250)+1);
        var b = Math.floor((Math.random()*250)+1);

        if (! face.hasMesh) {
            continue;
        }


        var entry =  {
                name: solid.getShapeName(face),
                color:(r*255+g)*255+b,
                mesh: face.mesh.toJSON()
        };
        entry.mesh.materials[0].colorDiffuse = [ Math.random(),Math.random(),Math.random()];
        responseObject.push( entry);
    }

    return responseObject;

}
exports.buildCSG1 = function(req,res)
{
    try {
        var  solidBuilderScript = decodeURIComponent(req.body.script);
        //xx console.log( "csg" , solidBuilderScript);

        var env = {
            csg: fastocc ,
            occ: fastocc ,
            shapeFactory: shapeFactory,

            'console':    { log: function() { 
			console.log.apply(console,arguments);
			logs.push(arguments);
			} },
            'eval':        function() { throw "eval is forbidden";        },
            'require':     function() { throw "require is forbidden";     },
            'setTimeout':  function() { throw "setTimeout is forbidden";  },
            'setInterval': function() { throw "setInterval is forbidden"; },
            solid: null ,
            error: null } ;
        // console.log("env = ",env );

        var code = "function buildSolid() { " + solidBuilderScript + "\n} solid = buildSolid();"

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

                        res.send(buildMesh(env.solid));

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

        if (1) {


        }else {
            // execute script in a special vm
            //xx code ="solid = 'toto';"
                        var jg = jailguard.create({timeout: 100});
                        jg.run(code, env, function(err) {
                            console.log("err = ",err);
                            console.log("env = ",env);

                            if (!err) {

                                res.send(buildMesh(env.solid));

                            } else {
                                console.log(" error in jailguard:", err,JSON.stringify(err));
                                res.send(501,"Error building solid : "+ err.message + err.toString());

                            }

                        });
        }


    }
    catch(err) {
        console.log(err.message + err.toString());
        res.send(501,"Error building solid : "+ err.message + "    "+ err.toString());
        return;
    }

}