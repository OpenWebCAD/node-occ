function makePanLid(r,h,H,thickness)
{

    var R =  (r*r+h*h)/(2*h);
    var center = [ 0, 0, H + h -R ];

    var outerSphere = csg.makeSphere(center,R);
    var innerSphere = csg.makeSphere(center,R-thickness);
    var solid = csg.cut(outerSphere,innerSphere);
    var cyl  = csg.makeCylinder([0,0, H + h -3*R], [0,0,H], R+r*2);
    solid = csg.cut(solid,cyl);
    return solid;
}
function makePan(_height,_radius)
{
    var height = _height  || 20;
    var radius  = _radius || 25.0;
    var thickness = 1;
    var handleRadius = 4;
    var handleLength = 30;

    var s1 = csg.makeCylinder([0,0,0],[0,0,height] , radius);

    var s2 = csg.makeSphere([0,0,0], radius);

    var s3 = csg.makeCylinder([0,0,-radius*0.7],[0,0,height], radius*2);
    var s4 = csg.fuse(s1,s2);
    var s5 = csg.common(s4,s3);

    var handle = csg.makeCylinder(
        [radius-2*thickness,0,height-handleRadius*1.1],
        [handleLength+radius-2*thickness,0,height-handleRadius*1.1],
        handleRadius );

    var s6 = csg.fuse(s5,handle);

    var r1 = csg.makeCylinder([0,0,0],[0,0,height] , radius-thickness);
    var r2 = csg.makeSphere([0,0,0],radius-thickness);
    var r3 = csg.makeCylinder( [0,0,-radius*0.7+thickness],
        [0,0,height], radius*2);
    var r4 = csg.fuse(r1,r2);
    var r5 = csg.common(r4,r3);

    var body = csg.cut(s6,r5);

    return body;

}

var lid = makePanLid(40,10, 20,1);
lid = lid.translate([0,0,20]);

var pan = makePan(20,40);

var solid = csg.fuse(pan,lid);

display(solid);

