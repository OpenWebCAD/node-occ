module.exports = function(grunt) {
  // Do grunt-related things in here
  grunt.initConfig({
     pkg:   grunt.file.readJSON('package.json'),

    jshint: {
      files: ['Gruntfile.js', 'lib/**/*.js', 'test/**/*.js'],
      options: {
        // options here to override JSHint defaults
        globals: {
          jQuery: true,
          console: true,
          module: true,
          document: true
        }
      }
    },
    
    
    
    mochacli: {
        options: {
          //reporter: 'nyan'        
          reporter: 'spec'        
        },
        all:  ['test/**/*_test.js']

    },


     concat: {
      options: {
        separator: ';'
      },
      dist: {
        src: ['lib/**/*.js'],
        dest: 'dist/<%= pkg.name %>.js'
      }
       
     },
     uglify: {
        options: {
          // the banner is inserted at the top of the output
          banner: '/*! <%= pkg.name %> <%= grunt.template.today("dd-mm-yyyy") %> */\n'
        },
        dist: {
          files: {
            'dist/<%= pkg.name %>.min.js': ['<%= concat.dist.dest %>']
          }
        }
      },

     docco: {
  debug: {
    src: ['lib/*.js','test/**/*.js'],
    options: {
      output: 'docs/'
    }
  }
        
     }
  });
  
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-contrib-concat');
  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-mocha-cli');
  grunt.loadNpmTasks('grunt-docco');
  grunt.registerTask('doc', ['docco']);

  grunt.registerTask('default', ['jshint','mochacli','concat','uglify']);// ['jshint', 'qunit', 'concat', 'uglify']);
};

