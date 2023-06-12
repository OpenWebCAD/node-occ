import should from "should";
import { ScriptRunner } from "..";

describe("Script Runner", function () {
    let scriptRunner: ScriptRunner;
    let myEnv = { foo: "bar" };

    before(() => {
        scriptRunner = new ScriptRunner(myEnv);
    });

    it("SR1 - should not be possible load external module with require", function (done) {

        scriptRunner.run("let fs= require('fs');", () => {
            should.fail(false, "done callback");
            done(new Error("test has failed: 'require' call hasn't been intercepted"));
        }, (err: Error) => {
            should.exist(err);
            err.message.should.equal("require is forbidden");
            done(null); //
        });
    });
    it("should not be possible to use eval", function (done) {

        scriptRunner.run("eval('a=10');",

            function () {
                should.fail(false, "done callback");
                done(new Error("test has failed: 'eval' call hasn't been intercepted"));
            },
            function error_callback(err: Error) {
                should.exist(err);
                err.message.should.equal("eval is forbidden");
                done(null); //
            }
        );

    });
    it("should expose environment provided by the caller", function (done) {

        scriptRunner.env.logs = []; // purge log
        scriptRunner.env.logs.length.should.equal(0);
        scriptRunner.env.foo.should.equal("bar");
        scriptRunner.run("console.log(foo); foo='baz'",

            function () {

                scriptRunner.env.foo.should.equal("baz");
                scriptRunner.env.logs.length.should.equal(1);

                done();
            },
            function error_callback(err: Error) {
                done(err); //  should not fail
            }
        );

    });
});
