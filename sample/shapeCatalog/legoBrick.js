
function makeRepetition(csg,shape,dX,nX,dY,nY) {
    var h1 = shape.clone() ;
     // installer la grille
    var shapeArray = [];
    var x=0,y=0;
    for (;y<nY;y++) {
        var hh1 = h1.clone();
        for (x=0;x<nX;x++) {                
            shapeArray.push(hh1);
            hh1 = hh1.translate([dX,0,0]);
        }    
        h1 = h1.translate([0,dY,0]);
    }  
    return csg.compound(shapeArray);
}

function makeLegoBrick(csg,nX,nY,h) {

    "use strict";
    
    if (h === 'thin') {
        h=2;
    } else if (h ==='thick') {
        h=6;
    } else {
      throw new Error("'invalid h");
    }
      

    var u = 1.6; // lego unit

    var outerWidth   = nX*u*5;
    var outerLength  = nY*u*5;
    var outerHeight  = h *u;
    
    var brick = csg.makeBox([0,0,0],[outerWidth,outerLength,outerHeight]);
    brick = csg.makeThickSolid(brick,brick.faces.bottom,-u);
    
    var h2 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight],
                [2.5*u,2.5*u,outerHeight+u],
                1.5*u);
    
    var tetons = makeRepetition(csg,h2,5*u,nX,5*u,nY);
    brick = csg.fuse(brick,tetons);   
    
    var h1 = csg.makeCylinder(
                [2.5*u,2.5*u,outerHeight-3*u],
                [2.5*u,2.5*u,outerHeight+0.75],
                0.74*u);   
    tetons = makeRepetition(csg,h1,5*u,nX,5*u,nY);  
    brick = csg.cut(brick,tetons);   

   
   // small pins
   var pinOuter  = csg.makeCylinder(
            [2.5*u,2.5*u,0],
            [2.5*u,2.5*u,outerHeight-u],
            u
    );
    var pinInner  = csg.makeCylinder(
            [2.5*u,2.5*u,0],
            [2.5*u,2.5*u,outerHeight-u],
            0.5*u
    );
    
    var pin = csg.cut(pinOuter,pinInner); 
  
    var p;
    if (nY == 1 ) {
        // install small pin insid
        p = pin.clone();
        p =p.translate([2.5*u,0,0]);
        tetons = makeRepetition(csg,p,5*u,nX-1,0,1);  
        brick = csg.fuse(brick,tetons);
        
    } else  if (nX == 1 ) {   
        
        p = pin.clone();
        p =p.translate([0,2.5*u,0]);        
        tetons = makeRepetition(csg,p,0,1,5*u,nY-1);
        brick = csg.fuse(brick,tetons);
        
    }  else if (nX > 1 && nY > 1) {        
        pinOuter  = csg.makeCylinder( [5*u,5*u,0], [5*u,5*u,outerHeight-u],4.07/2.0*u);
        pinInner  = csg.makeCylinder( [5*u,5*u,0],[5*u,5*u,outerHeight-u],1.5*u);
        pin = csg.cut(pinOuter,pinInner);        
        tetons = makeRepetition(csg,pin,5*u,nX-1,5*u,nY-1);
        brick = csg.fuse(brick,tetons);
    }
    
    return brick;
}
display(makeLegoBrick(occ,2,4,"thick"));
