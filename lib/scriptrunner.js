"use strict";
/* global require,console,require */
const fs    = require('fs');
const vm    = require('vm');
const util  = require('util');
const color = require('colors');
const getTemporaryFilePath = require('gettemporaryfilepath');
const doDebug = false;


function ScriptRunner(envparam) {
    let me = this;
    me.env = {


        'print': function() { 
            me.env.console.log.apply(me.env.console,arguments); 
        },
    
        'fnum': function(value,width,prec) {
            let a = value.toFixed(prec);
            let s = "             "+ a;
            return  s.slice(s.length-width);
        },

        'logs': [],
        
        'console': { 
                'log': function() {
                    console.log.apply(console,arguments);
                    me.env.logs.push(arguments);
                    // todo : raise an event
                }  
        },

        'eval':        function() { throw new Error("eval is forbidden");        },
        'require':     function() { throw new Error("require is forbidden");     },
        'setTimeout':  function() { throw new Error("setTimeout is forbidden");  },
        'setInterval': function() { throw new Error("setInterval is forbidden"); },
        error: null
    };

    // extend default env object with env parameters
    for(let p in envparam) {
        if (envparam.hasOwnProperty(p)) {
            if (doDebug) {
                console.log("adding object ",p);
            }
            me.env[p] = envparam[p];

        }
    }
}


function getLineNumber() {
    let original = Error.prepareStackTrace;
    let error = {};
    Error.captureStackTrace(error, getLineNumber);
    let lineNumber = error.stack;
    Error.prepareStackTrace = original;
    return lineNumber ;
}

ScriptRunner.prototype.run = function(code,done_callback,error_callback) {

    let settings = {
        prefix: "script",
        suffix: ".bar"
    };

    let me = this;
    let log = me.env.console.log;

    let filename = getTemporaryFilePath(settings);

    fs.writeFile(filename,code, function(err) {
        console.log(" executing script " + filename);
        try {

            vm.runInNewContext(code,me.env,filename);
            done_callback();
        }
        catch(_err) {

            console.trace("\n-------------------------------- EXCEPTION CAUGHT -----------------".red);

            log("transaction ended with an error",_err.message);
            log("error string = ", _err.toString() );
            log("error stack  = ", _err.stack);
            error_callback(_err);
        }
    });
};


exports.ScriptRunner = ScriptRunner;
