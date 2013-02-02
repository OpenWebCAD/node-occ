#include "Solid.h"

#include "Util.h"
#include "Face.h"
#include "Edge.h"
#include "BoundingBox.h"


Persistent<FunctionTemplate> Solid::constructor;




/*static*/
void Solid::Init(Handle<Object> target)
{

    // Prepare constructor template
    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Solid::New));
    constructor->SetClassName(String::NewSymbol("Solid"));

    // object has one internal filed ( the C++ object)
    constructor->InstanceTemplate()->SetInternalFieldCount(1);


    // Prototype
    Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

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
	


    EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numFaces);
    EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numSolids);
    EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numShells);

    EXPOSE_READ_ONLY_PROPERTY_DOUBLE (Solid,volume);
    EXPOSE_READ_ONLY_PROPERTY_DOUBLE (Solid,area);
    EXPOSE_READ_ONLY_PROPERTY(_mesh,mesh);

    target->Set(String::NewSymbol("Solid"), constructor->GetFunction());

}


void Solid::InitNew(const v8::Arguments& args)
{
	Shape::InitNew(args);
	REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Solid,area);
	REXPOSE_READ_ONLY_PROPERTY_DOUBLE(Solid,volume);

    REXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numFaces);
    REXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numSolids);
    REXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numShells);
}

Handle<v8::Value> Solid::New(const v8::Arguments& args)
{
    HandleScope scope;
    if (!args.IsConstructCall()) {
        ThrowException(Exception::TypeError(String::New(" use new occ.Solid() to construct a solid")));
        return scope.Close(Undefined());
    }

    Solid* pThis = new Solid();
    pThis->Wrap(args.This());
	pThis->InitNew(args);

    args.This()->Set(String::NewSymbol("faces"),          v8::Object::New() , (v8::PropertyAttribute)(v8::DontDelete|v8::ReadOnly)) ;
    args.This()->Set(String::NewSymbol("_reversedMap"),   v8::Object::New() , (v8::PropertyAttribute)(v8::DontEnum|v8::DontDelete|v8::ReadOnly));

	/// args.This()->SetAccessor(String::NewSymbol("_area"),ee< Solid, Number, double, &Solid::area>,0,Number::New(12),DEFAULT,None);

    // return scope.Close(args.This());
    return args.This();
}

Local<Object> Solid::Clone() const
{
	HandleScope scope;
	Handle<Object> instance = Solid::NewInstance()->ToObject();
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
	

    return scope.Close(instance);
}

Handle<Value> Solid::NewInstance()
{
    HandleScope scope;
    Local<Object> instance = Solid::constructor->GetFunction()->NewInstance(0,0);
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(instance);
    return scope.Close(instance);
}

Handle<Value> Solid::NewInstance(TopoDS_Shape shape)
{
    HandleScope scope;
    Local<Object> instance = Solid::constructor->GetFunction()->NewInstance(0,0);
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(instance);
    pThis->setShape(shape);
    return scope.Close(instance);
}

Handle<v8::Value> Solid::NewInstance(const v8::Arguments& args)
{
    TopoDS_Shape shape;
    return NewInstance(shape);
}


Handle<Value> Solid::getEdges(const v8::Arguments& args)
{

    HandleScope scope;
    // can work with this
    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    TopTools_IndexedMapOfShape map;
    // TopExp::MapShapes(pThis->shape(), TopAbs_EDGE, edgeMap);
    BRepTools::Map3DEdges(pThis->shape(), map);


    int nbShape =map.Extent();
    Local<Array> arr = Array::New(nbShape);

    for (int i=0; i<nbShape; i++)  {
        Local<Object> obj=  buildWrapper(map(i+1)); // 1 based !!!
        arr->Set(i,obj);
    }
    return scope.Close(arr);
}

Handle<v8::Value> Solid::getVertices(const v8::Arguments& args)
{
    HandleScope scope;
    // can work with this
    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    TopTools_IndexedMapOfShape map;
    TopExp::MapShapes(pThis->shape(), TopAbs_VERTEX, map);


    int nbShape =map.Extent();
    Local<Array> arr = Array::New(nbShape);

    for (int i=0; i<nbShape; i++)  {
        Local<Object> obj=  buildWrapper(map(i+1)); // 1 based !!!
        arr->Set(i,obj);
    }
    return scope.Close(arr);
}

Handle<v8::Value> Solid::getOuterShell(const v8::Arguments& args)
{
    HandleScope scope;
    // can work with this
    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    if (pThis->shape().ShapeType() == TopAbs_COMPOUND) {
        return scope.Close(Undefined());
    }
    try {
        TopoDS_Solid solid = TopoDS::Solid(pThis->shape());
        TopoDS_Shell shell = BRepTools::OuterShell(solid);
        return scope.Close(buildWrapper(shell));

    }
    CATCH_AND_RETHROW("Failed to extract Outer Shell ");

    return scope.Close(Undefined());
}

Handle<Value> Solid::getFaces(const v8::Arguments& args)
{

    HandleScope scope;
    // can work with this
    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    TopTools_IndexedMapOfShape shapeMap;
    TopExp::MapShapes(pThis->shape(), TopAbs_FACE, shapeMap);

    int nbSubShapes =shapeMap.Extent();
    Local<Array> arr = Array::New(nbSubShapes);

    for (int i=0; i<nbSubShapes; i++)  {
        Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
        arr->Set(i,obj);
    }
    return scope.Close(arr);
}


Handle<Value> Solid::getSolids(const v8::Arguments& args)
{

    HandleScope scope;
    // can work with this
    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    TopTools_IndexedMapOfShape shapeMap;
    TopExp::MapShapes(pThis->shape(), TopAbs_SOLID, shapeMap);

    int nbSubShapes =shapeMap.Extent();
    Local<Array> arr = Array::New(nbSubShapes);

    for (int i=0; i<nbSubShapes; i++)  {
        Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
        arr->Set(i,obj);
    }
    return scope.Close(arr);
}

Handle<Value> Solid::getShells(const v8::Arguments& args)
{

    HandleScope scope;
    // can work with this
    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    TopTools_IndexedMapOfShape shapeMap;
    TopExp::MapShapes(pThis->shape(), TopAbs_SHELL, shapeMap);

    int nbShapes =shapeMap.Extent();
    Local<Array> arr = Array::New(nbShapes);

    for (int i=0; i<nbShapes; i++)  {
        Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
        arr->Set(i,obj);
    }
    return scope.Close(arr);
}




/**
 * getAdjacentFace
 *  returns an array of all faces that are adjacents to the given face
 */
Handle<v8::Value> Solid::getAdjacentFaces(const v8::Arguments& args)
{
    HandleScope scope;
	
	Face* pFace = 0 ;
	if (!extractArg(args[0],pFace)) {
		ThrowException(Exception::TypeError(String::New("invalid arguments : expecting <FACE>")));
		return Handle<v8::Value>(Undefined());
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

    Local<Array> arr = Array::New(nbFaces);
	
	TopTools_MapIteratorOfMapOfShape it(auxmap);
	int i=0;
    for (; it.More(); it.Next()) {
		const TopoDS_Shape& shape= it.Key();
		Local<Object> obj= buildWrapper(shape); // 1 based !!!
		arr->Set(i,obj);
		i++;
    }
    return scope.Close(arr);

  // return Handle<v8::Value>(Undefined());
}


const char* getCommonEdges_Doc = "Solid.getCommonEdges(<Face>,<Face>);";

Handle<v8::Value> Solid::getCommonEdges(const v8::Arguments& args)
{
	// <face1>,<face2>
	HandleScope scope;
	Face* pFace1 = 0 ;
	Face* pFace2 = 0 ;


    Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        // create a new object
        ThrowException(Exception::Error(String::New("invalid object")));
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

	if (args.Length()<2 || !extractArg(args[0],pFace1) || !extractArg(args[1],pFace2) ) {
		ThrowException(Exception::TypeError(String::New("invalid arguments getCommonEdges : expecting <FACE>,<FACE>")));
		return Handle<v8::Value>(Undefined());
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

	Local<Array> arr = Array::New(edgeList.Extent());
	TopTools_MapIteratorOfMapOfShape it(edgeList);
	int i=0;
	for (; it.More(); it.Next()) {
		const TopoDS_Edge& edge = TopoDS::Edge(it.Key());
		Local<Object> obj= buildWrapper(edge); // 1 based !!!
		arr->Set(i++,obj);
	}
	return scope.Close(arr);
}

const char* getCommonVertices_Doc = "Solid.getCommonVertices(<Face>,<Face>);\n"
							    	 "Solid.getCommonVertices(<Face>,<Face>,<Face>);\n"
								    "Solid.getCommonVertices(<Edge>,<Edge>);\n";

Handle<v8::Value> Solid::getCommonVertices(const v8::Arguments& args)
{
	HandleScope scope;
	Face* pFace1 = 0 ;
	Face* pFace2 = 0 ;
	if (args.Length()<2 || !extractArg(args[0],pFace1) || !extractArg(args[1],pFace2) ) {
		ThrowException(Exception::TypeError(String::New("invalid arguments getCommonEdges : expecting <FACE>,<FACE>")));
		return Handle<v8::Value>(Undefined());
	}	
	Local<Array> arr = Array::New(0);
	return scope.Close(arr);
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




Handle<v8::Value> Solid::_mesh(Local<String> property,const AccessorInfo &info)
{
    HandleScope scope;
    if (info.This().IsEmpty()) {
        return scope.Close(Undefined());
    }
    if (info.This()->InternalFieldCount() == 0 ) {
        return scope.Close(Undefined());
    }
    Solid* pThis = ObjectWrap::Unwrap<Solid>(info.This());
    if (pThis->m_cacheMesh.IsEmpty()) {
        pThis->m_cacheMesh = Persistent<Object>::New(pThis->createMesh(0.1,5*3.14159/180.0,true));
    }
    return scope.Close(pThis->m_cacheMesh);
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
    HandleScope scope;

    const unsigned argc = 0;
    Handle<v8::Value> argv[1] = {  };
    Local<Object> theMesh = Mesh::constructor->GetFunction()->NewInstance(argc, argv);

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
    return scope.Close(theMesh);
}


Handle<v8::Value> Solid::getShapeName(const v8::Arguments& args)
{
    HandleScope scope;

	Handle<Object> pJhis = args.This();
    if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
        ThrowException(Exception::Error(String::New("invalid object")));
	    return scope.Close(Undefined());
    }
    Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

    Handle<Object> pShape = args[0]->ToObject();
    if (!pShape.IsEmpty()) {
        Handle<Value> hashCode = pShape->Get(String::NewSymbol("hashCode"));
        Handle<Object> reversedMap = pJhis->Get(String::NewSymbol("_reversedMap"))->ToObject();
		Handle<Value>  value = reversedMap->Get(hashCode);
        return scope.Close(value);
    }
    return scope.Close(Undefined());

}

std::string Solid::_getShapeName(const TopoDS_Shape& shape)
{
    v8::Handle<v8::Object>& pJhis = this->handle_;

    v8::Handle<v8::Object> reversedMap = pJhis->Get(v8::String::NewSymbol("_reversedMap"))->ToObject();
    v8::Handle<v8::Value> hashCode = v8::Integer::New(shape.HashCode(std::numeric_limits<int>::max()));
    v8::Handle<v8::Value> value = reversedMap->Get(hashCode);

    v8::String::AsciiValue asciiVal(value);

    return *asciiVal;
}

void Solid::_registerNamedShape(const char* name,const TopoDS_Shape& shape)
{
    if (shape.ShapeType() == TopAbs_FACE)  {
        Handle<Object> obj = this->handle_->Get(String::NewSymbol("faces"))->ToObject();
        obj->Set(String::NewSymbol(name),    Face::NewInstance(TopoDS::Face(shape)));
    }

    Handle<Object> reversedMap = this->handle_->Get(String::NewSymbol("_reversedMap"))->ToObject();
    reversedMap->Set(shape.HashCode(std::numeric_limits<int>::max()),String::NewSymbol(name));
}



