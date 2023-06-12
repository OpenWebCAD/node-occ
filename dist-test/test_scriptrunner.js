"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const should_1 = __importDefault(require("should"));
const __1 = require("..");
describe("Script Runner", function () {
    let scriptRunner;
    let myEnv = { foo: "bar" };
    before(function () {
        scriptRunner = new __1.ScriptRunner(myEnv);
    });
    it("should not be possible load external module with require", function (done) {
        scriptRunner.run("let fs= require('fs');", function () {
            should_1.default.fail(false, "done callback");
            done(new Error("test has failed: 'require' call hasn't been intercepted"));
        }, function error_callback(err) {
            should_1.default.exist(err);
            err.message.should.equal("require is forbidden");
            done(null); //
        });
    });
    it("should not be possible to use eval", function (done) {
        scriptRunner.run("eval('a=10');", function () {
            should_1.default.fail(false, "done callback");
            done(new Error("test has failed: 'eval' call hasn't been intercepted"));
        }, function error_callback(err) {
            should_1.default.exist(err);
            err.message.should.equal("eval is forbidden");
            done(null); //
        });
    });
    it("should expose environment provided by the caller", function (done) {
        scriptRunner.env.logs = []; // purge log
        scriptRunner.env.logs.length.should.equal(0);
        scriptRunner.env.foo.should.equal("bar");
        scriptRunner.run("console.log(foo); foo='baz'", function () {
            scriptRunner.env.foo.should.equal("baz");
            scriptRunner.env.logs.length.should.equal(1);
            done();
        }, function error_callback(err) {
            done(err); //  should not fail
        });
    });
});
