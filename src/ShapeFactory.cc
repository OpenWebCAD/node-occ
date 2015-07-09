#include "ShapeFactory.h"

#include "Solid.h"
#include "Edge.h"
#include "Face.h"
#include "Util.h"


#include <memory>
#include <limits>

#define  Primitives_Direction BRepPrim_Direction
#define  Primitives_XMin      BRepPrim_XMin
#define  Primitives_XMax      BRepPrim_XMax
#define  Primitives_YMin      BRepPrim_YMin
#define  Primitives_YMax      BRepPrim_YMax
#define  Primitives_ZMin      BRepPrim_ZMin
#define  Primitives_ZMax      BRepPrim_ZMax

char m(Primitives_Direction p)
{
  switch(p) {
    case Primitives_XMin: return 'x';
    case Primitives_YMin: return 'y';
    case Primitives_ZMin: return 'z';
    case Primitives_XMax: return 'X';
    case Primitives_YMax: return 'Y';
    case Primitives_ZMax: return 'Z';
  }
  return 0;
}
static void registerMakeBoxFaces(Solid* pThis,BRepPrimAPI_MakeBox& tool)
{
  pThis->_registerNamedShape("top",   tool.TopFace());
  pThis->_registerNamedShape("bottom",tool.BottomFace());
  pThis->_registerNamedShape("right", tool.RightFace());
  pThis->_registerNamedShape("left",  tool.LeftFace());
  pThis->_registerNamedShape("front", tool.FrontFace());
  pThis->_registerNamedShape("back",  tool.BackFace());

  BRepPrim_GWedge& wedge = tool.Wedge(); 

  for (int _p1 =Primitives_XMin; _p1 <=Primitives_YMax;_p1++) {
    Primitives_Direction p1=(Primitives_Direction)_p1;
    for (int _p2 = ((_p1>>1)+1)*2; _p2 <=Primitives_ZMax;_p2++) {
      Primitives_Direction p2=(Primitives_Direction)_p2;
      if (wedge.HasEdge(p1,p2)) {
        char name[4];
        name[0]='E';
        name[1]=m(p1);
        name[2]=m(p2);
        name[3]=0;
        pThis->_registerNamedShape(name,wedge.Edge(p1,p2));
      }  
      for (int _p3 = ((_p2>>1)+1)*2; _p3 <=Primitives_ZMax;_p3++) {
        Primitives_Direction p3=(Primitives_Direction)_p3;
        if(wedge.HasVertex(p1,p2,p3)) {
          char name[5];
          name[0]='V';
          name[1]=m(p1);
          name[2]=m(p2);
          name[3]=m(p3);
          name[4]=0;
          pThis->_registerNamedShape(name,wedge.Vertex(p1,p2,p3));
        }
      }
    }
  }
}


NAN_METHOD(ShapeFactory::makeBox)
{
  // could be :
  //    3 numbers dx,dy,dz
  //    2 points  p1,p2
  //TODO   1 point + 3 numbers dx,dy,dz
  //TODO   1 object with { x: 1,y: 2,z: 3, dw:
  NanScope();
  Handle<v8::Value> pJhis = Solid::NewInstance();
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

  double dx = 10;
  double dy = 10;
  double dz = 10;

  try {

    if (args.Length() == 3 && args[0]->IsNumber() && args[1]->IsNumber()  && args[2]->IsNumber() ) {

      dx  = args[0]->ToNumber()->Value();
      dy  = args[1]->ToNumber()->Value();
      dz  = args[2]->ToNumber()->Value();

      BRepPrimAPI_MakeBox  tool(dx, dy, dz);
      pThis->setShape(tool.Shape());
      registerMakeBoxFaces(pThis,tool);



    } else if (args.Length() == 2) {

      gp_Pnt p1;
      ReadPoint(args[0],&p1);

      gp_Pnt p2;
      ReadPoint(args[1],&p2);

      BRepPrimAPI_MakeBox tool(p1,p2);
      pThis->setShape(tool.Shape());
      registerMakeBoxFaces(pThis,tool);


    } else if (args.Length() == 3) {

      gp_Pnt p1;
      ReadPoint(args[0],&p1);

      dx  = args[2]->ToNumber()->Value();
      dy  = args[3]->ToNumber()->Value();
      dz  = args[4]->ToNumber()->Value();

      BRepPrimAPI_MakeBox tool(p1,dx, dy, dz);
      pThis->setShape(tool.Shape());
      registerMakeBoxFaces(pThis,tool);
    } else {
      NanThrowError("invalid arguments in makeBox");
    }
  } catch(Standard_Failure&) {
    NanThrowError("cannot build box");
  }
  NanReturnValue(pJhis);

}


static void registerMakeBoxFaces(Solid* pThis,BRepPrimAPI_MakePrism& tool)
{
  pThis->_registerNamedShape("bottom",   tool.FirstShape());
  pThis->_registerNamedShape("top",      tool.LastShape());

  // const TopTools_ListOfShape& list = tool.Generated(S);
}

NAN_METHOD(ShapeFactory::makePrism)
{
  NanScope();
  Handle<v8::Value> pJhis = Solid::NewInstance();
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

  // <FACE> [x,y,z]
  // <FACE> [x,y,z] [x,y,z]
  // OCCBase *shape, OCCStruct3d p1, OCCStruct3d p2)
  Face* pFace = DynamicCast<Face>(args[0]);

  gp_Vec direction(0,0,10);
  ReadVector(args[1],&direction);

  if (!pFace || direction.IsEqual(gp_Vec(0,0,0),1E-7,1E-8)) {
    NanThrowError("invalid arguments : expecting <FACE>,<VECTOR>");
  }

  try {
    const TopoDS_Shape& face = pFace->face();

    BRepPrimAPI_MakePrism prismMaker(face, direction);


    pThis->setShape(prismMaker.Shape());
    registerMakeBoxFaces(pThis,prismMaker);

    // possible fix shape
    if (!pThis->fixShape())   {
      StdFail_NotDone::Raise("Shapes not valid");
    }

  }
  CATCH_AND_RETHROW("Failed to create prism ");

  NanReturnValue(pJhis);
}

static void registerOneAxisFaces(Solid* pThis,BRepPrim_OneAxis& tool)
{
  pThis->_registerNamedShape("lateral",tool.LateralFace());
  if (tool.HasSides())   {
    pThis->_registerNamedShape("start",  tool.StartFace());
    pThis->_registerNamedShape("end",    tool.EndFace());
  }
  if (tool.HasTop())     {
    pThis->_registerNamedShape("top",    tool.TopFace());
  }
  if (tool.HasBottom())  {
    pThis->_registerNamedShape("bottom", tool.BottomFace());
  }
  /*
     TopoDS_Wire& AxisStartWire() ;
     TopoDS_Wire& EndWire() ;
     TopoDS_Wire& AxisEndWire() ;
     TopoDS_Edge& AxisEdge() ;
     TopoDS_Edge& StartEdge() ;
     TopoDS_Edge& EndEdge() ;
     TopoDS_Edge& StartTopEdge() ;
     TopoDS_Edge& StartBottomEdge() ;
     TopoDS_Edge& EndTopEdge() ;
     TopoDS_Edge& EndBottomEdge() ;
     TopoDS_Edge& TopEdge() ;
     TopoDS_Edge& BottomEdge() ;
     TopoDS_Vertex& AxisTopVertex() ;
     TopoDS_Vertex& AxisBottomVertex() ;
     TopoDS_Vertex& TopStartVertex() ;
     TopoDS_Vertex& TopEndVertex() ;
     TopoDS_Vertex& BottomStartVertex() ;
     TopoDS_Vertex& BottomEndVertex() ;
     */
}

NAN_METHOD(ShapeFactory::makeSphere)
{
  NanScope();
  Handle<v8::Value> pJhis = Solid::NewInstance();
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());


  gp_Pnt center(0,0,0);
  ReadPoint(args[0],&center);
  double radius = args[1]->ToNumber()->Value();
  if (radius < 1E-7) {
    NanThrowError("invalid radius");
    NanReturnValue(pJhis);
  }
  try {
    BRepPrimAPI_MakeSphere tool(center, radius);
    pThis->setShape(tool.Shape());
    registerOneAxisFaces(pThis,tool.Sphere());
  }
  CATCH_AND_RETHROW("Failed to create sphere ");

  NanReturnValue(pJhis);
}

bool ReadAx2(const Handle<Value>& value,gp_Ax2* ax2)
{
  assert(ax2);
  try {
    if (value->IsArray()) {
      Handle<Array> arr= Handle<Array>::Cast(value);
      gp_Pnt origin;
      ReadPoint(arr->Get(0),&origin);
      if (arr->Length() == 2) {
        // variation 2 :  gp_Ax2(const gp_Pnt& P,const gp_Dir& V);
        gp_Dir V;
        ReadDir(arr->Get(1),&V);
        *ax2 =  gp_Ax2(origin,V);
        return true;
      }
      if (arr->Length() == 3) {
        // variation 1 : gp_Ax2(const gp_Pnt& P,const gp_Dir& N,const gp_Dir& Vx);
        gp_Dir N,Vx;
        ReadDir(arr->Get(1),&N);
        ReadDir(arr->Get(2),&Vx);
        *ax2 =  gp_Ax2(origin,N,Vx);
        return true;
      }
    }
  }
  CATCH_AND_RETHROW("Failed to extract position");
  return false;
}

NAN_METHOD(ShapeFactory::makeCylinder)
{
  NanScope();
  Handle<v8::Value> pJhis = Solid::NewInstance();
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());


  //  gp_Ax2& Axes
  //  gp_Ax2(const gp_Pnt& P,const gp_Dir& N,const gp_Dir& Vx);
  // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const Standard_Real R,const Standard_Real H);
  // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const Standard_Real R,const Standard_Real H,const Standard_Real Angle);
  // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,const Standard_Real R,const Standard_Real H);
  // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,const Standard_Real R,const Standard_Real H,const Standard_Real Angle);
  const double epsilon = 1E-3;

  gp_Ax2 axis;
  if (args.Length()==2) {

    // variation 1   <R:number> <H:number>
    // a vertical cylinder of radius R starting a (0,0,0) ending at (0,0,H)
    double R  = args[0]->ToNumber()->Value();
    double H  = args[1]->ToNumber()->Value();

    if ( R < epsilon || H < epsilon ) {
      NanThrowError("invalid value for arguments");
    }
    try {
      pThis->setShape(BRepPrimAPI_MakeCylinder(R,H).Shape());
    }
    CATCH_AND_RETHROW("Failed to create cylinder ");

  } else if (args.Length()==3) {

    if (args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsNumber()) {

      // variation 2
      //  <[ <Origin[x,yz]>, <VZn[x,yz]>,<VXn[x,yz]>] <R:number> <H:number>
      gp_Ax2  ax2;
      bool success = ReadAx2(args[0],&ax2); (void)success;

      double R  = args[1]->ToNumber()->Value();
      double H  = args[2]->ToNumber()->Value();

      if ( R < epsilon || H < epsilon ) {
        NanThrowError("invalid value for arguments");
        NanReturnUndefined();
      }
      try {
        BRepPrimAPI_MakeCylinder tool(ax2,R,H);
        pThis->setShape(tool.Shape());
        registerOneAxisFaces(pThis,tool.Cylinder());
      }
      CATCH_AND_RETHROW("Failed to create cylinder ");

    } else if (args[0]->IsArray() && args[1]->IsArray() && args[2]->IsNumber()) {

      // variation 3 ( 2 points and a radius  )
      gp_Pnt p1;
      ReadPoint(args[0],&p1);

      gp_Pnt p2;
      ReadPoint(args[1],&p2);

      double R  = args[2]->ToNumber()->Value();

      const double dx = p2.X() - p1.X();
      const double dy = p2.Y() - p1.Y();
      const double dz = p2.Z() - p1.Z();

      const double H = sqrt(dx*dx + dy*dy + dz*dz);
      if (H < epsilon ) {
        NanThrowError("cannot build a cylinder on two coincident points");
        NanReturnUndefined();
      }
      gp_Vec aV(dx / H, dy / H, dz / H);
      gp_Ax2 ax2(p1, aV);
      try {
        BRepPrimAPI_MakeCylinder tool(ax2,R,H);
        pThis->setShape(tool.Shape());
        registerOneAxisFaces(pThis,tool.Cylinder());
      }
      CATCH_AND_RETHROW("Failed to create cylinder ");

    }
  } else {
    NanThrowError("invalid arguments");
    NanReturnUndefined();
  }

  NanReturnValue(pJhis);
}

NAN_METHOD(ShapeFactory::makeCone)
{
  NanScope();
  Handle<v8::Value> pJhis = Solid::NewInstance();
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

  const double epsilon = 1E-3;
  // Standard_EXPORT   BRepPrimAPI_MakeCone(const Standard_Real R1,const Standard_Real R2,const Standard_Real H);
  // Standard_EXPORT   BRepPrimAPI_MakeCone(const Standard_Real R1,const Standard_Real R2,const Standard_Real H,const Standard_Real angle);
  // Standard_EXPORT   BRepPrimAPI_MakeCone(const gp_Ax2& Axes,const Standard_Real R1,const Standard_Real R2,const Standard_Real H,const Standard_Real angle);
  if (args.Length()==3 && args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber()) {

    double R1 = args[0]->ToNumber()->Value();
    double R2 = args[1]->ToNumber()->Value();
    double H  = args[2]->ToNumber()->Value();

    if ( R1 < epsilon || R2 < epsilon || H < epsilon ) {
      NanThrowError("invalid value for arguments");
      NanReturnUndefined();
    }
    try {
      BRepPrimAPI_MakeCone tool(R1, R2,H);
      pThis->setShape(tool.Shape());
      registerOneAxisFaces(pThis,tool.Cone());
    }
    CATCH_AND_RETHROW("Failed to create Cone ");
  } else if (args.Length()==3 && args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsNumber()) {

    gp_Dir axis;
    ReadDir(args[0],&axis);

    double angleInRadian = 0;
    ReadDouble(args[1],angleInRadian);

    double height = 0;
    ReadDouble(args[1],height);

    NanThrowError("Cone with [u,v,w],angle,height not implemented yet");
    NanReturnUndefined();

  } else if (args.Length()==4 && args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsArray() && args[3]->IsNumber()) {
    // Point, point , R1,R2);
    // variation 3 ( 2 points and a radius  )
    gp_Pnt p1;
    ReadPoint(args[0],&p1);

    double R1  = 10;
    ReadDouble(args[1],R1);


    gp_Pnt p2;
    ReadPoint(args[2],&p2);

    double R2  = 11;
    ReadDouble(args[3],R2);

    const double dx = p2.X() - p1.X();
    const double dy = p2.Y() - p1.Y();
    const double dz = p2.Z() - p1.Z();

    const double H = sqrt(dx*dx + dy*dy + dz*dz);
    if (H < epsilon ) {
      NanThrowError("cannot build a cone on two coincident points");
      NanReturnUndefined();
    }
    gp_Vec aV(dx / H, dy / H, dz / H);
    gp_Ax2 ax2(p1, aV);
    try {
      BRepPrimAPI_MakeCone tool(ax2,R1,R2,H);
      pThis->setShape(tool.Shape());  
      registerOneAxisFaces(pThis,tool.Cone());
    }
    CATCH_AND_RETHROW("Failed to create cone ");

  } else if (args.Length()==4 && args[0]->IsArray() && args[1]->IsArray() && args[2]->IsNumber() && args[3]->IsNumber()) {
    // cone with a sharp apex 
    // apex, direction ,  half_angle, height
    gp_Pnt apex;
    ReadPoint(args[0],&apex);

    gp_Dir innerDir;
    ReadDir(args[1],&innerDir);

    double half_angle_in_radian=atan(1.0); // default : 45° 
    ReadDouble(args[2],half_angle_in_radian);

    double height = 100;
    ReadDouble(args[3],height);
    // r/h= tan(a);

    try {
      gp_Ax2 ax2(apex, innerDir);
      BRepPrimAPI_MakeCone tool(ax2,0,height*tan(half_angle_in_radian),height);
      pThis->setShape(tool.Shape());  
      registerOneAxisFaces(pThis,tool.Cone());
    }
    CATCH_AND_RETHROW("Failed to create cone with apex, direction , half_angle and height");

  } else {
    NanThrowError("invalid arguments (cone)");
  }

  NanReturnValue(pJhis);
}

NAN_METHOD(ShapeFactory::makeTorus)
{
  NanScope();
  Handle<v8::Value> pJhis = Solid::NewInstance();
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());

  // variation 1
  //  
  //  Center as <x,y,z>
  //  axis   as <u,v,w>
  //  bigRadius
  //  smallRadius
  if (args.Length()==4 && args[2]->IsNumber() && args[3]->IsNumber()) {
    //
    gp_Pnt center;
    ReadPoint(args[0],&center);
    //
    gp_Dir axis;
    ReadDir(args[1],&axis);

    double bigR = 100;
    ReadDouble(args[2],bigR);
    double smallR = 10;
    ReadDouble(args[3],smallR);

    try {
      BRepPrimAPI_MakeTorus tool(gp_Ax2(center,axis),bigR,smallR);
      pThis->setShape(tool.Shape());
      registerOneAxisFaces(pThis,tool.Torus());
      NanReturnValue(pJhis);
    }
    CATCH_AND_RETHROW("Failed to create Torus ");
  }
  NanThrowError("invalid arguments (makeTorus)");
  NanReturnUndefined();
}



#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>





class IShapeClassifierTool
{
  public:
    virtual const TopTools_ListOfShape& getGenerated(const TopoDS_Shape& shape) = 0;
    virtual const TopTools_ListOfShape& getModified(const TopoDS_Shape& shape)  = 0;
    virtual bool getDeleted(const TopoDS_Shape& shape)  = 0;
};
class IShapeNameAccessor
{
  public:
    virtual const TopoDS_Shape& shape() const = 0;
    virtual std::string getShapeName(const TopoDS_Shape& oldshape) =0; 
    virtual int operand() const =0;
};
class IShapeNameSetter
{
  public:
    virtual void setShapeName(const TopoDS_Shape& newshape,const char* name)=0;
};

class ShapeClassifier 
{
  public:
    ShapeClassifier(
        IShapeClassifierTool* tool,
        IShapeNameAccessor* nameAccessor1,
        IShapeNameAccessor* nameAccessor2, // optional : could be null
        IShapeNameSetter* nameSetter,
        const TopoDS_Shape& newShape);
    void classify();

  private:
    const TopoDS_Shape& m_newShape;
    IShapeClassifierTool* m_tool;
    IShapeNameAccessor*   m_nameAccessor1; //to get the name of a sub-shape of the old shape
    IShapeNameAccessor*   m_nameAccessor2; //to get the name of a sub-shape of the old shape
    IShapeNameSetter*     m_nameSetter;   //to set the name of a sub-shape on the new shape


    // sub-shape of new shape for which we have already computed a name
    TopTools_MapOfShape   m_processedSubShapes; 

    enum  ORIGIN { GENERATED, MODIFIED, IDENTICAL } ;

    void _classify(IShapeNameAccessor* originalBody,TopAbs_ShapeEnum shapeType);
    void _classifyRemainingSubShape(TopAbs_ShapeEnum shapeType);

    void registerShape(ORIGIN org,IShapeNameAccessor* originalBody,const TopoDS_Shape& originalShape,const TopoDS_Shape& newShape,int counter); 

    ShapeClassifier(const ShapeClassifier&);
    void operator=(const ShapeClassifier&);
};

ShapeClassifier::ShapeClassifier(
    IShapeClassifierTool* tool,
    IShapeNameAccessor* nameAccessor1,
    IShapeNameAccessor* nameAccessor2,
    IShapeNameSetter* nameSetter,
    const TopoDS_Shape& newShape
    )
  : m_newShape(newShape)
  , m_tool(tool)
  , m_nameAccessor1(nameAccessor1)
  , m_nameAccessor2(nameAccessor2)
  , m_nameSetter(nameSetter)
{

}


void ShapeClassifier::_classify(IShapeNameAccessor* obj,TopAbs_ShapeEnum shapeType)
{
  TopTools_IndexedMapOfShape newShapeMap;
  TopExp::MapShapes(this->m_newShape, shapeType, newShapeMap);

  TopTools_IndexedMapOfShape map;
  TopExp::MapShapes(obj->shape(), shapeType, map);

  for (int i=0;i<map.Extent();i++) {        
    const TopoDS_Shape& current = map.FindKey(i+1);

    int counterG = 0;
    int counterM = 0;
    const TopTools_ListOfShape& generatedShapes = m_tool->getGenerated(current);
    {
      TopTools_ListIteratorOfListOfShape it(generatedShapes);
      for (; it.More (); it.Next ()) {
        TopoDS_Shape& newShape = it.Value();
        if (!newShapeMap.Contains(newShape)) { continue; }
        if (this->m_processedSubShapes.Contains(newShape)) {
          continue; // already processed
        }
        registerShape(GENERATED,obj,current,newShape,counterG++);
      }
    }
    const TopTools_ListOfShape& modifiedShapes = m_tool->getModified(current);
    {
      TopTools_ListIteratorOfListOfShape it(modifiedShapes);
      for (; it.More (); it.Next ()) {
        TopoDS_Shape& newShape = it.Value();
        if (!newShapeMap.Contains(newShape)) { continue; }
        if (this->m_processedSubShapes.Contains(newShape)) {
          continue; // already processed
        }
        registerShape(MODIFIED,obj,current,newShape,counterM++);
      }
    }
    if ( (counterG + counterM == 0)  && !m_tool->getDeleted(current)) {
      if (!newShapeMap.Contains(current)) { continue; }
      registerShape(IDENTICAL,obj,current,current,-1);
    }
  }

}

void ShapeClassifier::_classifyRemainingSubShape(TopAbs_ShapeEnum shapeType)
{
  TopTools_IndexedMapOfShape oldShapeMap1;
  TopExp::MapShapes(m_nameAccessor1->shape(),shapeType,oldShapeMap1);
  TopTools_IndexedMapOfShape oldShapeMap2;
  if (m_nameAccessor2) {
    TopExp::MapShapes(m_nameAccessor2->shape(),shapeType,oldShapeMap2);
  }

  TopTools_IndexedMapOfShape map;
  TopExp::MapShapes(m_newShape,shapeType, map);
  for (int i=0;i<map.Extent();i++) {        
    const TopoDS_Shape& current = map.FindKey(i+1);

    if (this->m_processedSubShapes.Contains(current)) {
      continue; // already processed
    }
    if (this->m_tool->getDeleted(current)) {
      continue;
    }
    if ( oldShapeMap1.Contains(current) ) {
      // reuse name of old shape
      registerShape(IDENTICAL,m_nameAccessor1,current,current,-1);
    } else if ( oldShapeMap2.Contains(current) ) {
      // reuse name of old shape
      registerShape(IDENTICAL,m_nameAccessor2,current,current,-1);
    } else {
      // provide a default name based on hashCode
      std::stringstream s ;
      s << shapeType << "tmp" << current.HashCode(std::numeric_limits<int>::max());
      s << std::ends;
      m_nameSetter->setShapeName(current,s.str().c_str());
    }
  }
}
void ShapeClassifier::classify()
{
  _classify(m_nameAccessor1,TopAbs_FACE);
  _classify(m_nameAccessor1,TopAbs_EDGE);
  _classify(m_nameAccessor1,TopAbs_VERTEX);
  if (m_nameAccessor2) {
    _classify(m_nameAccessor2,TopAbs_FACE);
    _classify(m_nameAccessor2,TopAbs_EDGE);
    _classify(m_nameAccessor2,TopAbs_VERTEX);
  }



  // 
  // now check shape of the new solid that but that have'nt been processed yet
  //
  //   if the shape can be found in  old solid we can reuse the name of the
  //   old sub-shape.
  //   otherwise we give them a temporary name for completness
  //
  _classifyRemainingSubShape(TopAbs_FACE);
  _classifyRemainingSubShape(TopAbs_EDGE);
  _classifyRemainingSubShape(TopAbs_VERTEX);

}

void ShapeClassifier::registerShape(ORIGIN org,IShapeNameAccessor* nameAccessor,const TopoDS_Shape& originalShape,const TopoDS_Shape& newShape,int counter)
{

  std::string original_name = nameAccessor->getShapeName(originalShape);

  std::stringstream s ;

  std::string op;
  switch(org){
    case GENERATED:
      op ="g";
      break;
    case MODIFIED:
      op ="m";
      break;
    case IDENTICAL:
      break;
  }

  s << op ; 
  bool wantSep = false;
  if (nameAccessor->operand()>=0) {
    wantSep = true;
    s << nameAccessor->operand() ;
  }
  if (wantSep) {
    s << ":" ;
  }
  s << original_name;
  if (counter >=0) {
    s << ":" << counter ;
  }
  s << std::ends;

  std::string newName = s.str();

  m_processedSubShapes.Add(newShape);
  m_nameSetter->setShapeName(newShape,newName.c_str());

}

class BRepAlgoAPI_BooleanOperation_Adaptor: public IShapeClassifierTool
{
  public:
    BRepAlgoAPI_BooleanOperation_Adaptor(BRepAlgoAPI_BooleanOperation* pTool)
      :m_pTool(pTool)
    {
    };
    virtual const TopTools_ListOfShape& getGenerated(const TopoDS_Shape& current)
    {
      return m_pTool->Generated(current);
    };
    virtual const TopTools_ListOfShape& getModified(const TopoDS_Shape& current) 
    {
      return m_pTool->Modified(current);
    };
    virtual bool getDeleted(const TopoDS_Shape& shape) 
    {
      return m_pTool->IsDeleted(shape) ? true: false;
    };

    //
    BRepAlgoAPI_BooleanOperation* m_pTool;
};
class BRepBuilderAPI_MakeShape_Adapator: public IShapeClassifierTool
{
  public:
    BRepBuilderAPI_MakeShape_Adapator(BRepBuilderAPI_MakeShape* pTool)
      :m_pTool(pTool)
    {
    };
    virtual const TopTools_ListOfShape& getGenerated(const TopoDS_Shape& current)
    {
      return m_pTool->Generated(current);
    };
    virtual const TopTools_ListOfShape& getModified(const TopoDS_Shape& current) 
    {
      return m_pTool->Modified(current);
    };
    virtual bool getDeleted(const TopoDS_Shape& shape) 
    {
      return m_pTool->IsDeleted(shape) ? true: false;
    };

    //
    BRepBuilderAPI_MakeShape* m_pTool;
};

class ShapeNameAccessor : public  IShapeNameAccessor
{
  public:
    ShapeNameAccessor(Solid* obj,int operand = -1):m_operand(operand),m_obj(obj){};
    virtual const TopoDS_Shape& shape() const 
    {
      return m_obj->shape();
    }
    virtual std::string getShapeName(const TopoDS_Shape& shape)
    {
      std::string name = m_obj->_getShapeName(shape);
      return name;
    };
    virtual int operand() const { return m_operand;}
  private:
    Solid* m_obj;
    int m_operand;
};
class ShapeNameSetter : public  IShapeNameSetter
{
  public:
    ShapeNameSetter(Solid* obj):m_obj(obj){};
    virtual void setShapeName(const TopoDS_Shape& newshape,const char* name)
    {
      m_obj->_registerNamedShape(name,newshape);
    };
  private:
    Solid* m_obj;
};




static void registerShapes(BRepAlgoAPI_BooleanOperation* pTool,Solid* newSolid,Solid* oldSolid1,Solid* oldSolid2)
{
  const TopoDS_Shape& oldShape1 = oldSolid1->shape();
  const TopoDS_Shape& oldShape2 = oldSolid2->shape();
  const TopoDS_Shape& newShape = newSolid->shape();


  BRepAlgoAPI_BooleanOperation_Adaptor tool(pTool);
  ShapeNameAccessor  na1(oldSolid1,1);
  ShapeNameAccessor  na2(oldSolid2,2);
  ShapeNameSetter    ns(newSolid);

  ShapeClassifier classifier(&tool,&na1,&na2,&ns,newShape);

  classifier.classify();

}

static void registerShapes(BRepBuilderAPI_MakeShape* pTool,Solid* newSolid,Solid* oldSolid)
{
  const TopoDS_Shape& oldShape = oldSolid->shape();
  const TopoDS_Shape& newShape = newSolid->shape();

  BRepBuilderAPI_MakeShape_Adapator tool(pTool);
  ShapeNameAccessor  na(oldSolid);
  ShapeNameSetter    ns(newSolid);

  TopoDS_Shape empty;
  ShapeClassifier classifier(&tool,&na,NULL,&ns,newShape);

  classifier.classify();
}



static _NAN_METHOD_RETURN_TYPE ShapeFactory_createBoolean(_NAN_METHOD_ARGS,Solid* pSolid1, Solid* pSolid2, BOPAlgo_Operation op)
{
  NanScope();

  const TopoDS_Shape& firstObject= pSolid1->shape();
  const TopoDS_Shape& secondObject= pSolid2->shape();

  std::auto_ptr<BRepAlgoAPI_BooleanOperation> pTool;




  TopoDS_Shape shape;
  try {
    switch (op) {
      case BOPAlgo_FUSE:
        pTool = std::auto_ptr<BRepAlgoAPI_BooleanOperation>(new BRepAlgoAPI_Fuse(firstObject, secondObject));
        break;
      case BOPAlgo_CUT:
        pTool = std::auto_ptr<BRepAlgoAPI_BooleanOperation>(new BRepAlgoAPI_Cut(firstObject, secondObject));
        break;
      case BOPAlgo_COMMON:
        pTool = std::auto_ptr<BRepAlgoAPI_BooleanOperation>(new BRepAlgoAPI_Common(firstObject, secondObject));
        break;
      default:
        Standard_ConstructionError::Raise("unknown operation");
        break;
    }
    if (!pTool->IsDone()) {
      Standard_ConstructionError::Raise("operation failed");
    }
    shape = pTool->Shape();

    Handle<Value> result(Solid::NewInstance(shape));

    Solid* pResult = node::ObjectWrap::Unwrap<Solid>(result->ToObject());

    registerShapes(pTool.get(),pResult,pSolid1,pSolid2);

    if (pTool->HasDeleted())  {
      // the boolean operation causes some shape from s1 or s2 to be deleted
    }
    if (pTool->HasGenerated()) {
      // the boolean operation causes some shape from s1 or s2 to be created

    }
    if (pTool->HasModified()) {
      // the boolean operation causes some shape from s1 or s2 to be created
    }
    // check for empty compound shape
    TopoDS_Iterator It (shape, Standard_True, Standard_True);
    int found = 0;
    for (; It.More(); It.Next()) {
      found++;
    }
    if (found == 0) {
      Standard_ConstructionError::Raise("result object is empty compound");
    }

    // simplify compound with one solid into a Solid
    if (shape.ShapeType() == TopAbs_COMPOUND) {
      TopTools_IndexedMapOfShape shapeMap;
      TopExp::MapShapes(shape, TopAbs_SOLID, shapeMap);
      if (shapeMap.Extent() == 1) {
        pResult->setShape(shapeMap(1));
      }
    }
    NanReturnValue(result);

  }
  CATCH_AND_RETHROW("Failed in boolean operation");
  NanReturnUndefined();
}


Handle<v8::Value> ShapeFactory::add(const std::vector<Base*>& shapes)
{
  TopoDS_Compound compound;
  BRep_Builder builder;

  Handle<v8::Value> pJhis(Solid::NewInstance());
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis->ToObject());
  try {

    builder.MakeCompound(compound);

    for (size_t i = 0; i < shapes.size(); i++) {

      const TopoDS_Shape&  shape = shapes[i]->shape();
      builder.Add(compound, shape);
    }

    pThis->setShape(compound);

    std::cout << " num solid " << pThis->numSolids() << std::endl;

  }
  CATCH_AND_RETHROW("Failed in compound operation");

  return pJhis;

}

NAN_METHOD(ShapeFactory::compound)
{
  NanScope();
  std::vector<Base*> shapes;
  for (int i=0; i<args.Length(); i++) {
    Handle<Object> obj = args[i]->ToObject();
    if (NanHasInstance(Solid::_template,obj)) {
      Base* pShape = node::ObjectWrap::Unwrap<Solid>(obj);
      shapes.push_back(pShape);
    } else if (args[i]->IsArray()) {
      Handle<Array> arr = Handle<Array>::Cast(args[i]);
      int length = arr->Length();
      for(int j=0;j<length;j++) {
        Handle<Object> obj1 = arr->Get(j)->ToObject();
        if (NanHasInstance(Solid::_template,obj1)) {
          Base* pShape = node::ObjectWrap::Unwrap<Solid>(obj1);
          shapes.push_back(pShape);
        }
      }
    }
  }
  NanReturnValue(add(shapes));
}


_NAN_METHOD_RETURN_TYPE ShapeFactory::_boolean(_NAN_METHOD_ARGS,BOPAlgo_Operation op) {

  NanScope();

  if (!NanHasInstance(Solid::_template,args[0]) || !NanHasInstance(Solid::_template,args[1])) {
    NanThrowError("Wrong arguments for boolean operation : expecting two solids");
    NanReturnUndefined();
  }

  Solid* pSolid1 = node::ObjectWrap::Unwrap<Solid>(args[0]->ToObject());

  Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(args[1]->ToObject());
  /*
     std::vector<Solid*> other_solids;
     for (int i=1; i<args.Length(); i++) {
     if(Solid::constructor->HasInstance(args[i])) {
     Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(args[i]->ToObject());
     other_solids.push_back(pSolid2);
     }
     }
     Handle<Object> compound = ShapeFactory::add(other_solids);
     Solid* pSolid2 = node::ObjectWrap::Unwrap<Solid>(compound);
     */
  
  return ShapeFactory_createBoolean(args,pSolid1,pSolid2,op);

}

NAN_METHOD(ShapeFactory::fuse)
{
  return _boolean(args,BOPAlgo_FUSE);
}

NAN_METHOD(ShapeFactory::cut)
{
  return _boolean(args,BOPAlgo_CUT);
}

NAN_METHOD(ShapeFactory::common)
{
  return _boolean(args,BOPAlgo_COMMON);
}



bool extractListOfFaces(Local<v8::Value> value,TopTools_ListOfShape& faces)
{
  if (value->IsArray()) {
    Handle<Array> arr = Handle<Array>::Cast(value);
    int length = arr->Length();
    for (int i=0;i<length;i++) {
      Face* pFace =0;
      if (extractArg(arr->Get(i),pFace)) {
        faces.Append(pFace->face());
      }
    }
  } else {
    // could be a single face 
    Face* pFace =0;
    if (!extractArg(value,pFace)) {
      return false;
    }
    faces.Append(pFace->face());
  }


  return faces.Extent() >0;
}




NAN_METHOD(ShapeFactory::makeThickSolid)
{
  // variation 1 : <SOLID>,<FACE>,thickness
  // variation 2 : <SOLID>,[ <FACE> ... ],thickness
  NanScope();
  Solid* pSolid = 0;

  try {

    if(!extractArg(args[0],pSolid)) {
      NanThrowError("Wrong arguments for makeDraftAngle");
      NanReturnUndefined();
    }


    TopTools_ListOfShape faces;
    if (!extractListOfFaces(args[1],faces)) {
      NanThrowError("Wrong arguments for makeThickSolid");
      NanReturnUndefined();
    }

    double offset = 0;
    ReadDouble(args[2],offset);


    BRepOffset_Mode mode           = BRepOffset_Skin;
    Standard_Boolean bIntersection = Standard_False; 
    Standard_Boolean bSelfInter    = Standard_False;
    GeomAbs_JoinType joinType      = GeomAbs_Arc;
    double tol = 0.01;
    BRepOffsetAPI_MakeThickSolid tool(pSolid->solid(),faces,offset,tol,mode,bIntersection,bSelfInter,joinType);

    TopoDS_Shape shape = tool.Shape();

    Handle<Value> result(Solid::NewInstance(shape));

    Solid* pResult = node::ObjectWrap::Unwrap<Solid>(result->ToObject());


    registerShapes(&tool,pResult,pSolid);

    NanReturnValue(result); 

  }CATCH_AND_RETHROW("Failed in compound operation");
  NanReturnUndefined();
}


bool ReadPlane(const Local<v8::Value>& value,gp_Pln& plane)
{
  if (value.IsEmpty()) {
    return false;
  }
  // could be a planar face
  Face* pFace =0;
  if (!extractArg<Face>(value,pFace)) {
    return false;
  }

  Handle_Geom_Surface surf = BRep_Tool::Surface(pFace->face());
  GeomLib_IsPlanarSurface tool(surf);
  if (!tool.IsPlanar()) {
    return false;
  }
  plane = tool.Plan();

  return true;
}

NAN_METHOD(ShapeFactory::makeDraftAngle)
{

  // <SOLID>,(<FACE>|[<FACE>...]),<ANGLE>,<NeutralPlane>
  NanScope();

  try {
    if (args.Length() < 4) {
      NanThrowError("Wrong arguments for makeDraftAngle");
      NanReturnUndefined();
    }
    Solid* pSolid = 0;
    if(!extractArg(args[0],pSolid)) {
      NanThrowError("Wrong arguments for makeDraftAngle");
      NanReturnUndefined();
    }

    TopTools_ListOfShape faces;
    if(!extractListOfFaces(args[1],faces)) {
      NanThrowError("Wrong arguments for makeDraftAngle");
      NanReturnUndefined();
    }

    double angle = 10.0;
    ReadDouble(args[2],angle);

    //  gp_Dir direction;
    // ReadDir(args[3],&direction);

    gp_Pln neutralPlane;
    if (!ReadPlane(args[3],neutralPlane)) {
      // 
      NanThrowError("Wrong arguments for makeDraftAngle");
      NanReturnUndefined();
    }


    BRepOffsetAPI_DraftAngle tool(pSolid->shape());

    Standard_Boolean flag = Standard_False;

    int counter =0;
    TopTools_ListIteratorOfListOfShape it(faces);
    for (; it.More (); it.Next ()) {
      TopoDS_Face& face = TopoDS::Face(it.Value());

      gp_Dir direction =neutralPlane.Axis().Direction();
      tool.Add(face,direction,angle,neutralPlane,flag);
    }

    TopoDS_Shape shape = tool.Shape();

    Handle<Value> result(Solid::NewInstance(shape));

    Solid* pResult = node::ObjectWrap::Unwrap<Solid>(result->ToObject());


    registerShapes(&tool,pResult,pSolid);

    NanReturnValue(result); 

  } CATCH_AND_RETHROW("Failed in compound operation");
  NanReturnUndefined();


}


static int chamfer(Solid* pNewSolid,Solid* pSolid,const std::vector<Edge*>& edges, const std::vector<double>& distances)
{

  size_t edges_size = edges.size();
  size_t distances_size = distances.size();

  try {
    BRepFilletAPI_MakeChamfer CF(pSolid->shape());

    TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
    TopExp::MapShapesAndAncestors(pSolid->shape(), TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);

    for (size_t i=0; i<edges.size(); i++) {


      const TopoDS_Edge& edge = edges[i]->edge();

      // skip degenerated edge
      if (BRep_Tool::Degenerated(edge))
        continue;

      const TopoDS_Face& face = TopoDS::Face(mapEdgeFace.FindFromKey(edge).First());

      // skip edge if it is a seam
      if (BRep_Tool::IsClosed(edge, face))
        continue;


      if (distances_size == 1) {
        // single distance
        CF.Add(distances[0], edge, face);

      } else if (distances_size == edges_size) {
        // distance given for each edge
        CF.Add(distances[i], edge, face);

      } else {
        StdFail_NotDone::Raise("size of distances argument not correct");;
      }
    }

    CF.Build();

    if (!CF.IsDone())
      StdFail_NotDone::Raise("Failed to chamfer solid");

    const TopoDS_Shape& tmp = CF.Shape();

    if (tmp.IsNull())
      StdFail_NotDone::Raise("Chamfer operaton return Null shape");

    pNewSolid->setShape(tmp);

    // possible fix shape
    if (!pNewSolid->fixShape())
      StdFail_NotDone::Raise("Shapes not valid");

  }
  CATCH_AND_RETHROW("Failed to chamfer solid ");

  return 0;
}

static int fillet(Solid* pNewSolid,Solid* pSolid,const std::vector<Edge*>& edges,const  std::vector<double>& radius)
{
  size_t edges_size = edges.size();
  size_t radius_size = radius.size();

  try {
    BRepFilletAPI_MakeFillet tool(pSolid->shape());

    TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
    TopExp::MapShapesAndAncestors(pSolid->shape(), TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);

    for (size_t i=0; i<edges.size(); i++) {

      const TopoDS_Edge& edge = edges[i]->edge();

      // skip degenerated edge
      if (BRep_Tool::Degenerated(edge))
        continue;

      const TopoDS_Face& face = TopoDS::Face(mapEdgeFace.FindFromKey(edge).First());

      // skip edge if it is a seam
      if (BRep_Tool::IsClosed(edge, face))
        continue;

      if (radius_size == 1) {
        // single radius
        tool.Add(radius[0], edge);
      } else if (radius_size == edges_size) {
        // radius given for each edge
        tool.Add(radius[i], edge);
      } else if (radius_size == 2*edges_size) {
        // variable radius
        tool.Add(radius[2*i+0], radius[2*i+1], edge);
      } else {
        StdFail_NotDone::Raise("radius argument size not valid");;
      }
    }

    tool.Build();

    if (!tool.IsDone()) {
      StdFail_NotDone::Raise("Fillet operation has failed");
    }

    const TopoDS_Shape& tmp = tool.Shape();

    if (tmp.IsNull())  {
      StdFail_NotDone::Raise("Fillet operation resulted in Null shape");
    }

    pNewSolid->setShape(tmp);

    // possible fix shape
    //xx if (!pNewSolid->fixShape())    {
    //xx     StdFail_NotDone::Raise("Shapes not valid");
    //xx }
    registerShapes(&tool,pNewSolid,pSolid);

  }
  CATCH_AND_RETHROW("Failed to fillet solid ");

  return 1;

}


NAN_METHOD(ShapeFactory::makeFillet)
{

  NanScope();

  // <SOLID>, <EDGE> | [edges...],  radius | [ radii ]

  Solid* pSolid = 0;
  if(!extractArg(args[0],pSolid)) {
    NanThrowError("Wrong arguments for makeFillet");
    NanReturnUndefined();
  }
  std::vector<Edge*> edges;
  if (!_extractArray<Edge>(args[1],edges) || edges.size() ==0){
     NanThrowError("invalid arguments makeFillet: no edges provided:  expecting [<EDGE>...],radius");
     NanReturnValue(args.This()); 

  }

  std::vector<double> radii;
  if (args[2]->IsNumber()) {
    double radius = args[2]->ToNumber()->Value();
    if (radius < 1E-7 ) {
      //TODO
    }
    radii.push_back(radius);
  }

  Local<Object>  pNew = pSolid->Clone();

  Solid* pNewSolid = node::ObjectWrap::Unwrap<Solid>(pNew);

  fillet(pNewSolid,pSolid,edges,radii);

  NanReturnValue(pNew);

}

NAN_METHOD(ShapeFactory::makePipe)
{
  NanScope();
  NanThrowError("makePipe is currently unimplemented");
  NanReturnUndefined();
}




