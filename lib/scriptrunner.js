"use strict";
/* global require,console,require */
const fs = require("fs");
const vm = require("vm");
const util = require("util");
const getTemporaryFilePath = require("gettemporaryfilepath");
const doDebug = false;


function ScriptRunner(envparam) {
    const self = this;
    self.env = {


        "print": function () {
            self.env.console.log.apply(self.env.console, arguments);
        },

        "fnum": function (value, width, prec) {
            const a = value.toFixed(prec);
            const s = "             " + a;
            return s.slice(s.length - width);
        },

        "logs": [],

        "console": {
            "log": function () {
                /// console.log.apply(console,arguments);
                self.env.logs.push(arguments);
            }
        },

        "eval": function () {
            throw new Error("eval is forbidden");
        },
        "require": function () {
            throw new Error("require is forbidden");
        },
        "setTimeout": function () {
            throw new Error("setTimeout is forbidden");
        },
        "setInterval": function () {
            throw new Error("setInterval is forbidden");
        },
        error: null
    };

    // extend default env object with env parameters
    Object.keys(envparam).forEach(p => {
        if (envparam.hasOwnProperty(p)) {
            self.env[p] = envparam[p];
        }
    });
}


function getLineNumber() {
    const original = Error.prepareStackTrace;
    const error = {};
    Error.captureStackTrace(error, getLineNumber);
    const lineNumber = error.stack;
    Error.prepareStackTrace = original;
    return lineNumber;
}

ScriptRunner.prototype.run = function (code, done_callback, error_callback) {

    const settings = {
        prefix: "script",
        suffix: ".bar"
    };

    const self = this;
    const log = self.env.console.log;

    const filename = getTemporaryFilePath(settings);

    try {

        vm.runInNewContext(code, self.env, filename);
        done_callback();
    }
    catch (_err) {

        console.trace("\n-------------------------------- EXCEPTION CAUGHT -----------------");

        log("transaction ended with an error", _err.message);
        log("error string = ", _err.toString());
        log("error stack  = ", _err.stack);
        error_callback(_err);
    }
};


exports.ScriptRunner = ScriptRunner;
