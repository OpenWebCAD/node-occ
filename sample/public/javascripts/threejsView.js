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

    controls.staticMoving = true;
    controls.dynamicDampingFactor = 0.3;

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


function send_and_build_up_csg()
{
    var object = JSON.parse("{" + $("#csg").val() +"}");

    console.log(JSON.stringify(object,null," "));
    var s1 =JSON.stringify(object);
    var o1  = JSON.parse(s1);
    var s2 =JSON.stringify(o1);
    console.log(s2);

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
                 },/* texturePath */ undefined)
        });
}

