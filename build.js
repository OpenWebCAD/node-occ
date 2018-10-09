var exec = require('child_process').exec;

var command_line = './build.sh --only-prepare';
var environ = ""

if(process.platform === 'win32') {
  // tricks : https://github.com/remy/nodemon/issues/184#issuecomment-87378478 (Just don't add the space after the NODE_ENV variable, just straight to &&:)      
  command_line = 'set NODE_ENV=' + environ + '&& ' + command_line;
  return
} else {
  command_line = 'NODE_ENV=' + environ + ' ' + command_line;
}

var command = exec(command_line);

command.stdout.on('data', function(data) {
  process.stdout.write(data);
});
command.stderr.on('data', function(data) {
  process.stderr.write(data);
});
command.on('error', function(err) {
  process.stderr.write(err);
});
