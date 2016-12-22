var plate  = occ.makeBox([-100,-100,-10],[100,50,10]);

var holes  = [];

for (var i=0;i<10;i++) {
      for (var j=0;j<10;j++) {
                var hole = occ.makeCylinder([-100+10+i*10,-100+10+j*10,-20],[-100+10+i*10,-100+10+j*10,20],3);
                        holes.push(hole);
                            }
}

plate = occ.cut(plate,occ.compound(holes));
display(plate);
