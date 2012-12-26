
CSGTree = function ()
{
    this.counter = 0;
    this.operations ={};

};

CSGTree.prototype.__add = function (operatorName,args)
{
    this.counter+=1;
    var name = "s"+ this.counter;


    var argArr = [];
    // replace objects arguments with their names
    for (var a in args) {
        var arg = args[a];
        if (typeof arg === "object" && arg.hasOwnProperty("name")) {
            arg = arg["name"];
        }
        argArr.push(arg);
    }

    this.operations[name] = { name: name , type: operatorName , parameters: argArr };
    return this.operations[name];
}

CSGTree.prototype.makeCylinder = function() {  return this.__add("makeCylinder",arguments); };
CSGTree.prototype.makeCone     = function() {  return this.__add("makeCone",arguments);     };
CSGTree.prototype.makeBox      = function() {  return this.__add("makeBox",arguments);      };
CSGTree.prototype.makeSphere   = function() {  return this.__add("makeSphere",arguments);   };
CSGTree.prototype.fuse     = function() {  return this.__add("fuse",arguments);   };
CSGTree.prototype.common   = function() {  return this.__add("common",arguments);   };
CSGTree.prototype.cut      = function() {  return this.__add("cut",arguments);   };

CSGTree.prototype.toJSON = function()
{
    if (this.root === undefined) {
        var name = "s"+ this.counter;
        console.warn("setting root to " , name);
        this.root = this.operations[name];
    }
    this.operations.root = this.root["name"];
    return this.operations;
}

if (exports) { // exports may not be defined if this file is included in a HTML page
    exports.CSGTree =CSGTree;
}