function makeSomeShape()
{
    var s = occ.makeBox(10,20,30);
    return s;
}

var i, j,k;

var solids = [] ;

var shape = makeSomeShape();
for (i=0;i<5;i++){
    for (j=0;j<5;j++){
        for (k=0;k<5;k++){
            solids.push(shape.translate([i*100,j*100,k*100]));
        }
    }
}

var ret =  occ.compound(solids);
display(ret);

