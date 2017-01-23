const should = require('should');
const sr = require('../lib/scriptrunner');
describe("Script Runner",function()
{
    let runner;
	let myEnv = { foo: "bar" };
	
	before(function() {
		runner = new sr.ScriptRunner(myEnv);
	});
	
	it("should not be possible load external module with require",function(done){
		
		runner.run(                    "let fs= require('fs');",

			function(){
				should.fail("done callback");
				done(new Error("test has failed: 'require' call hasn't been intercepted"));
			},
			function error_callback(err){ 
				should.exist(err);
				err.message.should.equal("require is forbidden");
				done(null); // 
			}
		);
	});
	it("should not be possible to use eval",function(done){

		runner.run(                     "eval('a=10');",
			
			function(){
				should.fail("done callback");
				done(new Error("test has failed: 'eval' call hasn't been intercepted"));
			},
			function error_callback(err){ 
				should.exist(err);
				err.message.should.equal("eval is forbidden");
				done(null); // 
			}
		);

	});
	it("should expose environment provided by the caller",function(done){

		runner.env.logs = [] ; // purge log
		runner.env.logs.length.should.equal(0);
		runner.env.foo.should.equal("bar");
		runner.run(                     "console.log(foo); foo='baz'",
			
			function(){
				
				runner.env.foo.should.equal("baz");
				runner.env.logs.length.should.equal(1);

				done();
			},
			function error_callback(err){ 
				done(err); //  should not fail
			}
		);

	});
});
