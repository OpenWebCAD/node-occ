module.exports = function (grunt) {
    // Do grunt-related things in here
    grunt.initConfig({
        pkg: grunt.file.readJSON("package.json"),

        mochacli: {
            options: {
                //reporter: "nyan"
                reporter: "spec"
            },
            all: ["test/**/*_test.js"]

        },

        docco: {
            debug: {
                src: ["lib/*.js", "test/**/*.js"],
                options: {
                    output: "docs/"
                }
            }

        }
    });
    grunt.loadNpmTasks("grunt-mocha-cli");
    grunt.loadNpmTasks("grunt-docco");
    grunt.registerTask("doc", ["docco"]);

    grunt.registerTask("default", ["mochacli", "doc"]);
};

