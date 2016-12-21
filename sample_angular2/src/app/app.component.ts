import { Component, OnInit, AfterViewInit } from '@angular/core';
import { GeometryServiceService } from './services/geometry-service.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit, AfterViewInit {

  public aceId = "";
  collapse = false;
  editor: any;

  constructor(private geometryService: GeometryServiceService) {
  }

  ngOnInit() {
    this.aceId = "editor";
  }

  ngAfterViewInit() {
    this.editor = ace.edit("editor");
    this.editor.setTheme("ace/theme/twilight");
    this.editor.getSession().setMode("ace/mode/javascript");
    this.setEditorSample(1);

  }

  submitScript() {
    var code = this.editor.getValue();
    this.geometryService.postScript(code).subscribe((data) => {
        console.log('Success->', data);

        this.geometryService.doc.updateShapeObject(data, null);
        this.geometryService.doc.zoomExtens();
      },
      (error) => {
        console.log('Error->', error);
      });
  }

  zoomExtens() {
    this.geometryService.doc.zoomExtens();

  }

  toggleSideBar() {
    this.collapse = !this.collapse;
    this.geometryService.resizeViewport();
  }

  cleanScene()
  {
    this.geometryService.doc.new();
  }

  blankEditor() {
    this.editor.setValue('');
  }

  setEditorSample(sampleId) {
    switch (sampleId) {
      case 0:
        this.editor.setValue(`function makeSomeShape()
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
display(ret);`,1);

        break;

      case 1:
        this.editor.setValue(`function makeRepetition(csg,shape,dX,nX,dY,nY) {
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
`,1);
        break;
      case 2:
        this.editor.setValue(`function makePanLid(r,h,H,thickness)
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
        `,1);
        break;
      case 3:
        this.editor.setValue(`function makePiston() {
    return shapeFactory.makePiston(csg);
}

display(makePiston());
        `,1);
        break;
      case 4:
        this.editor.setValue(`var plate  = occ.makeBox([-100,-100,-10],[100,50,10]);

var holes  = [];

for (var i=0;i<10;i++) {
      for (var j=0;j<10;j++) {
                var hole = occ.makeCylinder([-100+10+i*10,-100+10+j*10,-20],[-100+10+i*10,-100+10+j*10,20],3);
                        holes.push(hole);
                            }
}

plate = occ.cut(plate,occ.compound(holes));
display(plate);
        `,1);
        break;

      default:
        this.editor.setValue(`
        `);
        break;


    }


  }


}

