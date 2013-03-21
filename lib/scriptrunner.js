var fs        = require('fs'),
    fileUtils = require ("file-utils");
var vm = require('vm');
var util = require('util');
var color = require('colors');
var File = fileUtils.File;
var SecurityManager = fileUtils.SecurityManager;

function getBlockedFunctionConstructor() {
    "use strict";

    function FakeFunction(execCode) {
        var code = "FUNCTION_CONSTRUCTOR_DETECTED";
        var message = "function constructor with code: " + execCode;
        var error = {code: code, message: message};

        return function() {};
    }

    return FakeFunction;
}


ScriptRunner = function(envparam) {
    

    var me = this;
    me.env = {

        'print': function() { 
            me.env.console.log.apply(me.env.console,arguments); 
        },
    
        fnum: function(value,width,prec) {
            var a = value.toFixed(prec);
            var s = "             "+ a;
            return  s.slice(s.length-width);
        },

        logs: [],
        
        'console': { 
                log: function() { 
                    console.log.apply(console,arguments);
                    me.env.logs.push(arguments);
                    // todo : raise an event
                }  
        },

        'eval':        function() { throw "eval is forbidden";        },
        'require':     function() { throw "require is forbidden";     },
        'setTimeout':  function() { throw "setTimeout is forbidden";  },
        'setInterval': function() { throw "setInterval is forbidden"; },
        error: null
    };

    // extend default env object with env paramaters
    for(var p in envparam) {
        console.log("adding object ",p);
        me.env[p] = envparam[p]; 
    }
};


ScriptRunner.prototype.run = function(code,done_callback,error_callback) {

    var settings = {
        prefix: "script",
        suffix: ".bar",
        directory: "/temp/"
    };

    var me = this;
    var log = me.env.console.log;

    File.createTempFile (settings, function (error, file) {
        
        if (error) {
            log("INTERNAL ERROR: cannot created temporary file");
            error_callback(error);
            return;
        }

        var filename = file.toString();

        fs.writeFile(filename,code, function(err) {
                console.log(" executing script " + filename);
                try {

                    vm.runInNewContext(code,me.env,filename);
                    done_callback();
                }
                catch(_err) {

                    console.trace("\n-------------------------------- EXCEPTION COUGHT -----------------".red);

                    function getLineNumber() {
                        var original = Error.prepareStackTrace;
                        var error = {};
                        Error.captureStackTrace(error, getLineNumber);
                        var lineNumber = error.stack;
                        Error.prepareStackTrace = original;
                        return lineNumber ;
                    }

                    log("transaction ended with an error",_err.message);
                    log("error string = ", _err.toString() );
                    log("error stack  = ", _err.stack);
                    error_callback(_err);
                }
        });
    });
};


exports.ScriptRunner = ScriptRunner;
