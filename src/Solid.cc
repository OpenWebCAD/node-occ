#include "Solid.h"

#include "Util.h"
#include "Face.h"
#include "Edge.h"
#include "BoundingBox.h"


Persistent<FunctionTemplate> Solid::_template;




/*static*/
void Solid::Init(Handle<Object> target)
{

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(Solid::New);  
  tpl->SetClassName(NanNew("Solid"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);

  // Prototype
  Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

  Base::InitProto(proto);

  EXPOSE_METHOD(Solid,getEdges);
  EXPOSE_METHOD(Solid,getVertices); 
  EXPOSE_METHOD(Solid,getFaces);
  EXPOSE_METHOD(Solid,getOuterShell);
  EXPOSE_METHOD(Solid,getShells);
  EXPOSE_METHOD(Solid,getSolids);
  EXPOSE_METHOD(Solid,getShapeName);
  EXPOSE_METHOD(Solid,getAdjacentFaces);
  EXPOSE_METHOD(Solid,getCommonEdges);	

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE (Solid,area);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE (Solid,volume);

  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numFaces);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numSolids);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numShells);

  EXPOSE_READ_ONLY_PROPERTY(_mesh,mesh);

  target->Set(NanNew("Solid"), tpl->GetFunction());

}


void Solid::InitNew(_NAN_METHOD_ARGS)
{
  Shape::InitNew(args);
  REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Solid,area);
  REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Solid,volume);
  REXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numFaces);
  REXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numSolids);
  REXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numShells);
}


NAN_METHOD(Solid::New)
{
  NanScope();
  if (!args.IsConstructCall()) {
    NanThrowError(" use new occ.Solid() to construct a solid");
    NanReturnUndefined();
  }

  Solid* pThis = new Solid();
  pThis->Wrap(args.This());
  pThis->InitNew(args);

  args.This()->ForceSet(NanNew("faces"),          NanNew<v8::Object>() , (v8::PropertyAttribute)(v8::DontDelete|v8::ReadOnly)) ;
  args.This()->ForceSet(NanNew("_reversedMap"),   NanNew<v8::Object>() , (v8::PropertyAttribute)(v8::DontEnum|v8::DontDelete|v8::ReadOnly));

  /// args.This()->SetAccessor(NanSymbol("_area"),ee< Solid, Number, double, &Solid::area>,0,Number::New(12),DEFAULT,None);

  // return scope.Close(args.This());
  NanReturnValue(args.This());
}

Local<Object> Solid::Clone() const
{
  //xx Local<Object> instance = NanNew(_template)->GetFunction()->NewInstance();
  Local<Object> instance = Solid::NewInstance()->ToObject();
  Solid* pClone = node::ObjectWrap::Unwrap<Solid>(instance);

  pClone->setShape(this->shape());


  if (!this->shape().IsNull()) {
    TopTools_IndexedMapOfShape shapeMap;
    TopExp::MapShapes(this->shape(), TopAbs_FACE, shapeMap);

    int nbSubShapes =shapeMap.Extent();


    for (int i=0; i<nbSubShapes; i++)  {
      int hc = shapeMap(i+1).HashCode(std::numeric_limits<int>::max());

      // TODO pClone->_registerNamedShape(name,
    }
  }


  return instance;
}

Handle<Value> Solid::NewInstance()
{
  Local<Object> instance = NanNew(Solid::_template)->GetFunction()->NewInstance(0,0);
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(instance);
  return instance;
}

Handle<Value> Solid::NewInstance(TopoDS_Shape shape)
{
  Local<Object> instance = NanNew(Solid::_template)->GetFunction()->NewInstance(0,0);
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(instance);
  pThis->setShape(shape);
  return instance;
}

NAN_METHOD(Solid::NewInstance)
{
  NanScope();
  TopoDS_Shape shape;
  NanReturnValue(NewInstance(shape));
}


NAN_METHOD(Solid::getEdges)
{

  NanScope();
  // can work with this
  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  TopTools_IndexedMapOfShape map;
  // TopExp::MapShapes(pThis->shape(), TopAbs_EDGE, edgeMap);
  BRepTools::Map3DEdges(pThis->shape(), map);


  int nbShape =map.Extent();
  Local<Array> arr = NanNew<Array>(nbShape);

  for (int i=0; i<nbShape; i++)  {
    Local<Object> obj=  buildWrapper(map(i+1)); // 1 based !!!
    arr->Set(i,obj);
  }
  NanReturnValue(arr);
}

NAN_METHOD(Solid::getVertices)
{
  NanScope();
  // can work with this
  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  TopTools_IndexedMapOfShape map;
  TopExp::MapShapes(pThis->shape(), TopAbs_VERTEX, map);


  int nbShape =map.Extent();
  Local<Array> arr = NanNew<Array>(nbShape);

  for (int i=0; i<nbShape; i++)  {
    Local<Object> obj=  buildWrapper(map(i+1)); // 1 based !!!
    arr->Set(i,obj);
  }
  NanReturnValue(arr);
}

NAN_METHOD(Solid::getOuterShell)
{
  NanScope();
  // can work with this
  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  if (pThis->shape().ShapeType() == TopAbs_COMPOUND) {
    NanReturnUndefined();
  }
  try {
    TopoDS_Solid solid = TopoDS::Solid(pThis->shape());
    // TopoDS_Shell shell = BRepTools::OuterShell(solid);
    TopoDS_Shell shell = OUTER_SHELL(solid);
    NanReturnValue(buildWrapper(shell));

  }
  CATCH_AND_RETHROW("Failed to extract Outer Shell ");

  NanReturnUndefined();
}

NAN_METHOD(Solid::getFaces)
{

  NanScope();
  // can work with this
  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  TopTools_IndexedMapOfShape shapeMap;
  TopExp::MapShapes(pThis->shape(), TopAbs_FACE, shapeMap);

  int nbSubShapes =shapeMap.Extent();
  Local<Array> arr = NanNew<Array>(nbSubShapes);

  for (int i=0; i<nbSubShapes; i++)  {
    Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
    arr->Set(i,obj);
  }
  NanReturnValue(arr);
}


NAN_METHOD(Solid::getSolids)
{

  NanScope();
  // can work with this
  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  TopTools_IndexedMapOfShape shapeMap;
  TopExp::MapShapes(pThis->shape(), TopAbs_SOLID, shapeMap);

  int nbSubShapes =shapeMap.Extent();
  Local<Array> arr = NanNew<Array>(nbSubShapes);

  for (int i=0; i<nbSubShapes; i++)  {
    Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
    arr->Set(i,obj);
  }
  NanReturnValue(arr);
}

NAN_METHOD(Solid::getShells)
{

  NanScope();
  // can work with this
  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
    NanReturnUndefined();
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  TopTools_IndexedMapOfShape shapeMap;
  TopExp::MapShapes(pThis->shape(), TopAbs_SHELL, shapeMap);

  int nbShapes =shapeMap.Extent();
  Local<Array> arr = NanNew<Array>(nbShapes);

  for (int i=0; i<nbShapes; i++)  {
    Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
    arr->Set(i,obj);
  }
  NanReturnValue(arr);
}



/**
 * getAdjacentFace
 *  returns an array of all faces that are adjacents to the given face
 */
NAN_METHOD(Solid::getAdjacentFaces)
{
  NanScope();

  Face* pFace = 0 ;
  if (!extractArg(args[0],pFace)) {
    NanThrowError("invalid arguments : expecting <FACE>");
    NanReturnUndefined();
  }
  assert(pFace);

  // arguments : <face>  , face must belongs to solid
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(args.This());


  TopTools_IndexedDataMapOfShapeListOfShape map;
  TopExp::MapShapesAndAncestors(pThis->shape(),TopAbs_EDGE,TopAbs_FACE,map);

  TopTools_MapOfShape auxmap;

  TopExp_Explorer edgeExplorer(pFace->shape(),TopAbs_EDGE);

  for (; edgeExplorer.More(); edgeExplorer.Next()) {
    TopoDS_Edge edge = TopoDS::Edge(edgeExplorer.Current());
    const TopTools_ListOfShape& list = map.FindFromKey(edge);

    // list of faces sh
    TopTools_ListIteratorOfListOfShape it(list);

    for (; it.More(); it.Next()) {

      if (pFace->shape() !=  it.Value()) {
        if(!auxmap.Contains(it.Value())) {
          auxmap.Add(it.Value());
        }
      }
    }
  }

  // now build an array with the answer
  int nbFaces = auxmap.Extent();

  Local<Array> arr = NanNew<Array>(nbFaces);

  TopTools_MapIteratorOfMapOfShape it(auxmap);
  int i=0;
  for (; it.More(); it.Next()) {
    const TopoDS_Shape& shape= it.Key();
    Local<Object> obj= buildWrapper(shape); // 1 based !!!
    arr->Set(i,obj);
    i++;
  }
  NanReturnValue(arr);
  // return Handle<v8::Value>(Undefined());
}


const char* getCommonEdges_Doc = "Solid.getCommonEdges(<Face>,<Face>);";

NAN_METHOD(Solid::getCommonEdges)
{
  // <face1>,<face2>
  NanScope();
  Face* pFace1 = 0 ;
  Face* pFace2 = 0 ;


  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    // create a new object
    NanThrowError("invalid object");
    NanReturnUndefined();
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  if (args.Length()<2 || !extractArg(args[0],pFace1) || !extractArg(args[1],pFace2) ) {
    NanThrowError("invalid arguments getCommonEdges : expecting <FACE>,<FACE>");
    NanReturnUndefined();
  }

  TopTools_IndexedDataMapOfShapeListOfShape map;

  TopExp::MapShapesAndAncestors(pThis->shape(), TopAbs_EDGE, TopAbs_FACE, map);

  TopTools_MapOfShape edgeList;

  TopExp_Explorer edgeExplorer(pThis->shape(),TopAbs_EDGE);
  for (; edgeExplorer.More(); edgeExplorer.Next()) {

    TopoDS_Edge edge = TopoDS::Edge(edgeExplorer.Current());

    const TopTools_ListOfShape& list = map.FindFromKey(edge);
    TopTools_ListIteratorOfListOfShape it(list);

    int nbFound = 0;
    for (; it.More(); it.Next()) {
      if (pFace1->shape() ==  it.Value()) { nbFound++; continue; }
      if (pFace2->shape() ==  it.Value()) { nbFound++; continue; }
    }
    if (nbFound == 2) {
      // this is the common edge
      edgeList.Add(edge);
    }

  }

  Local<Array> arr = NanNew<Array>(edgeList.Extent());
  TopTools_MapIteratorOfMapOfShape it(edgeList);
  int i=0;
  for (; it.More(); it.Next()) {
    const TopoDS_Edge& edge = TopoDS::Edge(it.Key());
    Local<Object> obj= buildWrapper(edge); // 1 based !!!
    arr->Set(i++,obj);
  }
  NanReturnValue(arr);
}


const char* getCommonVertices_Doc = "Solid.getCommonVertices(<Face>,<Face>);\n"
"Solid.getCommonVertices(<Face>,<Face>,<Face>);\n"
"Solid.getCommonVertices(<Edge>,<Edge>);\n";

NAN_METHOD(Solid::getCommonVertices)
{
  NanScope();
  Face* pFace1 = 0 ;
  Face* pFace2 = 0 ;
  if (args.Length()<2 || !extractArg(args[0],pFace1) || !extractArg(args[1],pFace2) ) {
    NanThrowError("invalid arguments getCommonEdges : expecting <FACE>,<FACE>");
    NanReturnUndefined();
  }	
  Local<Array> arr = Array::New(0);
  NanThrowError("Not Implemented ");
  NanReturnValue(arr);
}


int Solid::numSolids()
{
  const TopoDS_Shape& shp = this->shape();
  if (shp.IsNull()) {
    return 0;
  }

  if (shp.ShapeType() == TopAbs_SOLID) {
    return 1;
  } else {
    // CompSolid or Compound
    TopTools_IndexedMapOfShape compsolids;
    TopExp::MapShapes(shp, TopAbs_COMPSOLID, compsolids);

    TopTools_IndexedMapOfShape solids;
    TopExp::MapShapes(shp, TopAbs_SOLID, solids);

    return solids.Extent() + compsolids.Extent();
  }
}


int Solid::numFaces()
{
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->shape(), TopAbs_FACE, anIndices);
  return anIndices.Extent();
}

int Solid::numShells()
{
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(this->shape(), TopAbs_SHELL, anIndices);
  return anIndices.Extent();
}

double Solid::area()
{
  GProp_GProps prop;
  BRepGProp::SurfaceProperties(this->shape(), prop);
  return prop.Mass();
}

double Solid::volume()
{
  GProp_GProps prop;
  BRepGProp::VolumeProperties(this->shape(), prop);
  return prop.Mass();
}

//DVec Solid::inertia() {
//    DVec ret;
//    GProp_GProps prop;
//    BRepGProp::VolumeProperties(this->shape(), prop);
//    gp_Mat mat = prop.MatrixOfInertia();
//    ret.push_back(mat(1,1)); // Ixx
//    ret.push_back(mat(2,2)); // Iyy
//    ret.push_back(mat(3,3)); // Izz
//    ret.push_back(mat(1,2)); // Ixy
//    ret.push_back(mat(1,3)); // Ixz
//    ret.push_back(mat(2,3)); // Iyz
//    return ret;
//}

//OCCStruct3d OCCSolid::centreOfMass() {
//    OCCStruct3d ret;
//    GProp_GProps prop;
//    BRepGProp::VolumeProperties(this->getSolid(), prop);
//    gp_Pnt cg = prop.CentreOfMass();
//    ret.x = cg.X();
//    ret.y = cg.Y();
//    ret.z = cg.Z();
//    return ret;
//}


NAN_PROPERTY_GETTER(Solid::_mesh)
{
  NanScope();
  if (args.This().IsEmpty()) {
    NanReturnUndefined();
  }
  if (args.This()->InternalFieldCount() == 0 ) {
    NanReturnUndefined();
  }
  Solid* pThis = ObjectWrap::Unwrap<Solid>(args.This());
  if (pThis->m_cacheMesh.IsEmpty()) {
    NanAssignPersistent<v8::Object>(pThis->m_cacheMesh, pThis->createMesh(0.5,20*3.14159/180.0,true));
  }
  NanReturnValue(NanNew(pThis->m_cacheMesh));
}


//void Solid::Mesh()
//{
//    TopExp_Explorer Ex;
//    int numFaces = 0;
//    for (Ex.Init(shape_, TopAbs_FACE); Ex.More(); Ex.Next()) {
//        ++numFaces;
//    }
//
//    if (numFaces > 0) {
//        BRepMesh().Mesh(shape_, 1.0);
//    }
//}
Handle<Object>  Solid::createMesh(double factor, double angle, bool qualityNormals)
{
  NanEscapableScope();

  const unsigned argc = 0;
  Handle<v8::Value> argv[1] = {  };
  Local<Object> theMesh = NanNew(Mesh::_template)->GetFunction()->NewInstance(argc, argv);

  Mesh *mesh =  Mesh::Unwrap<Mesh>(theMesh);

  const TopoDS_Shape& shape = this->shape();

  try {
    BRepMesh_IncrementalMesh MSH(shape,factor,Standard_True,angle,Standard_True);

    /*
       Bnd_Box aBox;
       BRepBndLib::Add(shape, aBox);

       Standard_Real aXmin, aYmin, aZmin;
       Standard_Real aXmax, aYmax, aZmax;
       aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

       Standard_Real maxd = fabs(aXmax - aXmin);
       maxd = std::max(maxd, fabs(aYmax - aYmin));
       maxd = std::max(maxd, fabs(aZmax - aZmin));

       BRepMesh_FastDiscret MSH(factor*maxd, angle, aBox, Standard_True, Standard_True,Standard_True, Standard_True);
       MSH.Perform(shape);
       */

    if (shape.ShapeType() == TopAbs_COMPSOLID || shape.ShapeType() == TopAbs_COMPOUND) {
      TopExp_Explorer exSolid, exFace;
      for (exSolid.Init(shape, TopAbs_SOLID); exSolid.More(); exSolid.Next()) {
        const TopoDS_Solid& solid = TopoDS::Solid(exSolid.Current());
        for (exFace.Init(solid, TopAbs_FACE); exFace.More(); exFace.Next()) {
          const TopoDS_Face& face = TopoDS::Face(exFace.Current());
          if (face.IsNull()) continue;
          mesh->extractFaceMesh(face, qualityNormals);
        }
      }
    }  else {
      TopExp_Explorer exFace;
      for (exFace.Init(shape, TopAbs_FACE); exFace.More(); exFace.Next()) {
        const TopoDS_Face& face = TopoDS::Face(exFace.Current());
        if (face.IsNull()) continue;
        mesh->extractFaceMesh(face, qualityNormals);
      }
    }
  } CATCH_AND_RETHROW("Failed to mesh solid ");
  mesh->optimize();

  return NanEscapeScope(theMesh);
}


NAN_METHOD(Solid::getShapeName)
{
  NanScope();

  Handle<Object> pJhis = args.This();
  if ( pJhis.IsEmpty() || !NanHasInstance(_template,pJhis))  {
    NanThrowError("invalid object");
    NanReturnUndefined();
  }
  Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

  Handle<Object> pShape = args[0]->ToObject();
  if (!pShape.IsEmpty()) {
    v8::Local<v8::Value> hashCode = pShape->Get(NanNew("hashCode"));
    v8::Local<v8::Object> reversedMap = pJhis->Get(NanNew("_reversedMap"))->ToObject();
    v8::Local<v8::Value>  value = reversedMap->Get(hashCode);
    NanReturnValue(value);
  }
  NanReturnUndefined();

}

std::string Solid::_getShapeName(const TopoDS_Shape& shape)
{
  v8::Local<v8::Object> pJhis = NanObjectWrapHandle(this);

  v8::Local<v8::Object> reversedMap = pJhis->Get(NanNew("_reversedMap"))->ToObject();
  v8::Local<v8::Value> hashCode = NanNew<v8::Integer>(shape.HashCode(std::numeric_limits<int>::max()));
  v8::Local<v8::Value> value = reversedMap->Get(hashCode);

  NanAsciiString s(value);
  std::string res(*s);
  return res;
}

void Solid::_registerNamedShape(const char* name,const TopoDS_Shape& shape)
{
  if (shape.ShapeType() == TopAbs_FACE)  {
    Local<Object> obj = NanObjectWrapHandle(this)->Get(NanNew("faces"))->ToObject();
    obj->Set(NanNew(name),    Face::NewInstance(TopoDS::Face(shape)));
  }

  Local<Object> reversedMap = NanObjectWrapHandle(this)->Get(NanNew("_reversedMap"))->ToObject();
  reversedMap->Set(shape.HashCode(std::numeric_limits<int>::max()),NanNew(name));
}



