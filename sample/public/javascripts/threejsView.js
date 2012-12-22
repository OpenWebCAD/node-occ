/**
 *
 *
 */
var container,
    scene,
    renderer,
    camera,
    control;

$(document).ready(function() {

    installEditor();

    container = $("#3DView");

    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(75, container.width()/container.height(), 0.1, 1000);
    camera.position.z = 400;
    controls = new THREE.TrackballControls( camera ,container[0]);


    controls.rotateSpeed = 1.0;
    controls.zoomSpeed = 1.2;
    controls.panSpeed = 0.8;

    controls.noZoom = false;
    controls.noPan = false;

    controls.staticMoving = false;
    controls.dynamicDampingFactor = 0.3;
    var radius =100;
    controls.minDistance = radius * 1.1;
    controls.maxDistance = radius * 100;

    controls.keys = [ 65, 83, 68 ];

    controls.addEventListener( 'change', render );




    renderer =  new THREE.WebGLRenderer( { antialias: true, clearColor: 0xFF0000, clearAlpha: 1 } );
    renderer.setClearColorHex(0xEEEEEE, 1.0);
    renderer.clear();

    container.append(renderer.domElement);

    for (var x= -1 ; x<2;x=x+2) {
        for (var y= -1 ; y<2;y=y+2) {
            for (var z= -1 ; z<2;z=z+2) {
                var pointLight = new THREE.PointLight( 0xFFFFFF,0.2 );
                pointLight.position.x = 1000 * x;
                pointLight.position.y = 1000 * y;
                pointLight.position.z = 1000 * y;
                scene.add(pointLight);
            }
        }
    }




    var light = new THREE.AmbientLight( 0x222222 );
    scene.add( light );
    // -----------------------------
    var sphereMaterial = new THREE.MeshLambertMaterial({color: 0xCC0000 });

    var radius = 50, segments = 16, rings = 16;

    var sphere = new THREE.Mesh(
        new THREE.SphereGeometry(radius, segments, rings ),
        sphereMaterial
    );


    //xx scene.add(sphere);

    animate();

    onWindowResize();


    $("#button").click( send_and_build_up_csg);

});

function onWindowResize( event ) {


    SCREEN_WIDTH = container.width();
    SCREEN_HEIGHT = container.height();


    renderer.setSize( SCREEN_WIDTH, SCREEN_HEIGHT );
    camera.aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    camera.updateProjectionMatrix();

}

function animate() {
    requestAnimationFrame( animate );
    render();
    controls.update();
    //xx stats.update();
}

function render()
{
    renderer.render( scene, camera );
}
function shapeCenterOfGravity(geometry)
{
    geometry.computeBoundingBox();

    var bb = geometry.boundingBox;

    var center = new THREE.Vector3();

    center.add( bb.min, bb.max );
    center.multiplyScalar( -0.5 );

    return center;
}
var editor;
var delay;
function installEditor()
{
    editor = CodeMirror.fromTextArea(document.getElementById("code"), {
            lineNumbers: true,
            theme: "ambiance",
            matchBrackets: true,
            extraKeys: {
                "Enter": "newlineAndIndentContinueComment" ,
                "Ctrl-Space": "autocomplete"
            }});
    CodeMirror.commands.autocomplete = function(cm) {
            CodeMirror.simpleHint(cm, CodeMirror.javascriptHint);
    }

    editor.on("change", function() {
            clearTimeout(delay);
            delay = setTimeout(updatePreview, 2000);
    });

    setTimeout(updatePreview, 2000);
}

function updatePreview() {
    send_and_build_up_csg();

}


function send_and_build_up_csg()
{
    var object;
    var err_txt= "";
    $("#ascii_mesh").text(err_txt);
    try {

        // using simple text area
        // var code = $("#code").val();
        // /var myCodeMirror = CodeMirror.fromTextArea( $("#code")[0]);
        var code = editor.getValue();
            code = " var csg = new CSGTree();" + "var b__ = function() { " + code + "}; b__(); return csg; " ;

        // interpret the script
        var solid = new Function(code)();

        err_txt= '';
    } catch (e) {
        err_txt= 'Error: <code>' + e.toString().replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</code>';
        $("#ascii_mesh").text(err_txt);
        return;
    }
    object = solid;


    $.ajax({
        url: "/csg" ,
        data: JSON.stringify(object), // { object: object } ,
        type: "POST",
        contentType: "application/json",
        cache: false,
        dataType:"json",
        error: function(err) {
            var obj = JSON.parse(err.responseText)
            //xx alert(" error " + JSON.stringify(obj,null," "));
            $("#ascii_mesh").text(obj.error.stack);
        }
    }).done(



        function(json) {

            var beautified = JSON.stringify(json,null," ");
            $("#ascii_mesh").text(beautified);
            json.scale = 0.1;
            var jsonLoader = new THREE.JSONLoader();
             model = jsonLoader.createModel( json,
                 function(geometry,material ){
                     var sphereMaterial = new THREE.MeshLambertMaterial({color: 0xCC0000 });

                     var oldObj = scene.getChildByName("CSG");
                     if (oldObj) { scene.remove(oldObj); }
                     var newObj = new THREE.Mesh(geometry,sphereMaterial);
                     newObj.name ="CSG";
                     scene.add(newObj);

                     camera.lookAt(shapeCenterOfGravity(geometry));

                 },/* texturePath */ undefined)
        });
}

