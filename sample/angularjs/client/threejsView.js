/**
 *
 *
 */

function bytesToSize(bytes, precision) {
    var sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
    var posttxt = 0;
    if (bytes == 0) return 'n/a';
    if (bytes < 1024) {
            return Number(bytes) + " " + sizes[posttxt];
    }
    while( bytes >= 1024 ) {
            posttxt++;
            bytes = bytes / 1024;
    }
    return bytes.toPrecision(precision) + " " + sizes[posttxt];
}


var editor,  myLayout;

function installLayout() {
    // $('#container').layout({ applyDemoStyles: true });
    myLayout = $('body').layout(
        {
            west__onresize:    function(x, ui) {
                editor.resize();
            },
            center__onresize:  function(x, ui) {
                // resize webgl view
                onWindowResize();
            }
        }
    );
    myLayout.sizePane("west", 400);
}

var view;

$(document).ready(function() {
    "use strict";

    installLayout();

    installEditor();

    installSpinnerOnAjaxCall();


    var container = $("#graphical_view");

    if (container.size() === 0 ) {
        throw Error("Cannot find graphical view div");
    }    
    view = new GEOMVIEW(container[0]);

    view.on("animate",function() { updateAJS(); });

    $("#button").click(send_and_build_up_csg);

    $("#zoomAll").click(function() { 
         view.zoomAll();
    });

});

function onWindowResize( event ) {
    view && view.resizeRenderer();
}

var lastAjaxStart ;
var lastAjaxDuration;
var delay;

function restoreUserSession() {
    var rawData = localStorage.getItem("myscript");
    if (!rawData) return;
    var script = decodeURIComponent(rawData);
    if (script && script!="null") {
        editor.setValue(script);
        editor.gotoLine(1,1,false);
        editor.clearSelection();        
    }
}

function saveUserSession() {
    var script = encodeURIComponent(editor.getValue());
    if (script) {
        localStorage.setItem("myscript",script);
    }
}

function script_isValid() {

    var lint_errors = [];
    // editor.emit("jslint", lint_errors);

    var annotations = editor.getSession().getAnnotations();
    if (Object.keys(annotations).length == 0 ) {
        return true;
    } else {
        return false;
    }
}

function installACEEditor() {

    editor = ace.edit("editor");
    editor.isModified = false;
    editor.setTheme("ace/theme/monokai");
    editor.getSession().setMode("ace/mode/javascript");
    editor.getSession().on('changeAnnotation',function () {
        if (script_isValid()) {
            if (editor.isModified) {
                $("#button").show();
            }
        } else {
            $("#button").hide();            
        }
    });
    editor.getSession().on('change', function(e) {
        editor.isModified = true;
        clearTimeout(delay);
        delay = setTimeout(function() {

        }, 400);
        // e.type, etc
    });

    restoreUserSession();

    updatePreview();

}

function installEditor() { return installACEEditor(); }

function updatePreview() {
    if (editor.isModified)  {
        send_and_build_up_csg();
        editor.isModified=false;
        $("#button").hide();        
    }
}

function handle_json_error(request, statusText, errorThrown) {

    "use strict";
    console.log(request);
    lastAjaxDuration = new Date() - lastAjaxStart;

    $("#ascii_mesh").text(request.responseText + " duration :  " + lastAjaxDuration + " ms");


}



function install_json_mesh(json,size_in_byte) {

    "use strict";

    view.clearAll();
    view.updateShapeObject(json,function() {
        view.zoomAll();
    });
    

    // log
    json.logs.forEach(function(line){
        var str = "";
        for ( var a in line ) {
            str += line[a];
        }
        $("#ascii_mesh").append("<p>" + str + "</p>");
    })

}

/**
 * In send_and_build_up_csg_method1 the construction script is passed to the 
 * server as a text string. The provided script is executed by the server in a 
 * sandbox to produce the corresponding mesh.
 */
function send_and_build_up_csg_method2() {

    "use strict";
    
    var encoded_script = encodeURIComponent(editor.getValue());

    saveUserSession();

    lastAjaxStart = new Date();

    var size_in_byte = 0;
  
    $("#ascii_mesh").text("");
 

    var loaded = 0;
    var startTime  =0;
    var endTime = 0;

    $.ajax({
        url: "/csg1" ,
        data: JSON.stringify({ script: encoded_script}),
        type: "POST",
        contentType: "application/json",
        cache: false,
        dataType:"text",
        statusCode: {
            404: function() {
                $("#response").html('Could not contact server.');
            },
            500: function() {
                $("#response").html('A server-side error has occurred.');
            }
        },
        xhr: function() {
           var xhr = new window.XMLHttpRequest();

           // http://www.dave-bond.com/blog/2010/01/JQuery-ajax-progress-HMTL5/
           // http://www.w3.org/TR/XMLHttpRequest/#the-response-attribute
           xhr.addEventListener("loadstart",function(evt) {
             console.log(" load start");
	     startTime = new Date;
           },false);
           xhr.addEventListener("loadend",function(evt) {
             console.log(" load end");
             console.log( " loading " , evt.loaded , " in " , (endTime - startTime ) / 1.0 , " milliseconds");
           },false);

           xhr.addEventListener("progress", function on_progress(evt) {
                 if (evt.lengthComputable) {
                   var percentComplete = evt.loaded / evt.total;
                   console.log(" percent complete : ", percentComplete);
                 }
                 console.log(" loaded = " , evt.loaded);
                 loaded = evt.loaded;
	   },false);
          return xhr;
        },
        success: function(response,statusText,jqXHR) {
            endTime = new Date
            console.log("Success");
            if(response){       
                size_in_byte =  response.length ;
                lastAjaxDuration = new Date() - lastAjaxStart;
                $("#ascii_mesh").append("<p>duration: " + lastAjaxDuration + " ms   - size :" + bytesToSize(size_in_byte,3) + " loaded " + bytesToSize(loaded) + " </p>");
                $("#ascii_mesh").append("<p> loading : " + (endTime-startTime) + " ms  </p>");
 
            }
        },
        error: handle_json_error
    }).done( function(json) {
            var t1 = new Date();
            var obj = JSON.parse(json);
            var t2 = new Date();
            install_json_mesh(obj);
            var t3 = new Date();
             $("#ascii_mesh").append("<p>parsing duration: " + (t2-t1)  + " ms " + " " + (t3-t2)  + " ms </p>");
     });
}

/**
 * In send_and_build_up_csg_method1 the construction script is executed on the
 * javascript engine of the client browser to produce a JSON object that 
 * contains the modeling instructions. The JSON data structure is sent to the
 * erver to produce the mesh of the corresponding solid
 */
function send_and_build_up_csg_method1() {
    "use strict";
    var object;
    var err_txt= "";
    $("#ascii_mesh").text(err_txt);
    try {

        // using simple text area
        // var code = $("#code").val();
        // /var myCodeMirror = CodeMirror.fromTextArea( $("#code")[0]);
        var code = editor.getValue();
            code =  "var csg = new CSGTree();" +"var b__ = function() { " + code +     "}; b__(); return csg; " ;

        // interpret the script
        object = new Function(code)();

        err_txt= '';
    } catch (e) {
        err_txt= 'Error: <code>' + e.toString().replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</code>';
        $("#ascii_mesh").text(err_txt);
        return;
    }

    $.ajax({
        url: "/csg" ,
        data: JSON.stringify(object),
        type: "POST",
        contentType: "application/json",
        cache: false,
        dataType:"json",
        error: handle_json_error
    }).done( function(json) { install_json_mesh(json); });
}


function send_and_build_up_csg() {
    "use strict";
    send_and_build_up_csg_method2();
}


function SceneCtrl($scope) {
    "use strict";
/*
    $scope.getCamera =function() {
        return camera;
    };

    $scope.COG = view.COG; // center of gravity
    //xx $scope.__defineGetter__("camera", function(){

    //xx     return camera;
    //xx });
    $scope.camera1 = {};
    $scope.camera1.position = {};

    $scope.camera1.position.x = camera.position.x;
    $scope.camera1.position.y = camera.position.y;
    $scope.camera1.position.z = camera.position.z;
*/
}

function updateAJS() {
    // get Angular scope from the known DOM element
    var e = document.getElementById('myAngularApp');
    var scope = angular.element(e).scope();

    SceneCtrl(scope);
    scope.$apply();
}


var lastJQueryStart ;

function installSpinnerOnAjaxCall()
{

    $('#loadingDiv')
    .hide()  // hide it initially
    .ajaxStart(function() {
        $(this).show();
        lastJQueryStart = new Date();
    })
    .ajaxStop(function() {
        $(this).hide();
        lastAjaxDuration=new Date() - lastJQueryStart;
    });
}
