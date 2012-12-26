#include "Solid.h"

#include "Util.h"
#include "Face.h"
#include "Edge.h"
#include "BoundingBox.h"


Persistent<FunctionTemplate> Solid::constructor;



    
/*static*/void Solid::Init(Handle<Object> target)
{

  // Prepare constructor template
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Solid::New));
  constructor->SetClassName(String::NewSymbol("Solid"));

  // object has one internal filed ( the C++ object)
  constructor->InstanceTemplate()->SetInternalFieldCount(1);


  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

  Base::InitProto(proto);


  EXPOSE_METHOD(Solid,fillet);
  EXPOSE_METHOD(Solid,chamfer);

  EXPOSE_METHOD(Solid,getEdges);
  EXPOSE_METHOD(Solid,getFaces);
  EXPOSE_METHOD(Solid,getOuterShell);
  EXPOSE_METHOD(Solid,getShells);
  EXPOSE_METHOD(Solid,getSolids);
  EXPOSE_METHOD(Solid,getBoundingBox);

  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numFaces);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numSolids);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numShells);

  EXPOSE_READ_ONLY_PROPERTY_DOUBLE (Solid,volume);
  EXPOSE_READ_ONLY_PROPERTY_DOUBLE (Solid,area);
  EXPOSE_READ_ONLY_PROPERTY(Solid,_mesh,mesh);
  
  target->Set(String::NewSymbol("Solid"), constructor->GetFunction());

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
  // return scope.Close(args.This());
  return args.This();
}

Local<Object>  Solid::Clone()
{
  HandleScope scope;
  Solid* obj = new Solid();
  Local<Object> instance = constructor->GetFunction()->NewInstance();
  obj->Wrap(instance);
  obj->setShape(this->shape());
  return scope.Close(instance);
}	

Handle<Value> Solid::NewInstance(TopoDS_Shape& shape)
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

    TopTools_IndexedMapOfShape edgeMap;
    // TopExp::MapShapes(pThis->shape(), TopAbs_EDGE, edgeMap);
	BRepTools::Map3DEdges(pThis->shape(), edgeMap);


	int nbEdges =edgeMap.Extent(); 
	Local<Array> arr = Array::New(nbEdges);

	for (int i=0;i<nbEdges;i++)  {
		Local<Object> obj=  buildWrapper(edgeMap(i+1)); // 1 based !!!
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

	 } CATCH_AND_RETHROW("Failed to extract Outer Shell ");

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

	for (int i=0;i<nbSubShapes;i++)  {
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

	for (int i=0;i<nbSubShapes;i++)  {
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

	for (int i=0;i<nbShapes;i++)  {
		Local<Object> obj=  buildWrapper(shapeMap(i+1)); // 1 based !!!
		arr->Set(i,obj);
	}
	return scope.Close(arr);
}


int Solid::chamfer(const std::vector<Edge*>& edges, const std::vector<double>& distances)
{

    size_t edges_size = edges.size();
    size_t distances_size = distances.size();
    
    try {
        BRepFilletAPI_MakeChamfer CF(this->shape());
        
        TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
        TopExp::MapShapesAndAncestors(this->shape(), TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);
        
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
        
        this->setShape(tmp);
        
        // possible fix shape
        if (!this->fixShape())
            StdFail_NotDone::Raise("Shapes not valid");
        
     } CATCH_AND_RETHROW("Failed to chamfer solid ");

	return 0;
}

int Solid::fillet(const std::vector<Edge*>& edges,const  std::vector<double>& radius) 
{
    size_t edges_size = edges.size();
    size_t radius_size = radius.size();
    
    try {
        BRepFilletAPI_MakeFillet fill(this->shape());
        
        TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
        TopExp::MapShapesAndAncestors(this->shape(), TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);
        
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
                fill.Add(radius[0], edge);
            } else if (radius_size == edges_size) {
                // radius given for each edge
                fill.Add(radius[i], edge);
            } else if (radius_size == 2*edges_size) {
                // variable radius
                fill.Add(radius[2*i+0], radius[2*i+1], edge);
            } else {
                StdFail_NotDone::Raise("radius argument size not valid");;
            }
        }
        
        fill.Build();
        
        if (!fill.IsDone()) {
            StdFail_NotDone::Raise("Filler operation failed");
		}
        
        const TopoDS_Shape& tmp = fill.Shape();
        
        if (tmp.IsNull())  {
            StdFail_NotDone::Raise("Fillet operation resulted in Null shape");
		}
        
        this->setShape(tmp);
        
        // possible fix shape
        if (!this->fixShape())	{
            StdFail_NotDone::Raise("Shapes not valid");
		}
        
     } CATCH_AND_RETHROW("Failed to fillet solid ");

	return 1;
    
}
Handle<v8::Value> Solid::fillet(const v8::Arguments& args) 
{
	HandleScope scope;
	// [ <list of edges ], radius ]
	// [ <list of edges ], radius ]
	std::vector<Edge*> edges;
	extractArray<Edge>(args[0],edges);
	std::vector<double> radii;

	if (args[1]->IsNumber()) {	
		double radius = args[1]->ToNumber()->Value();
		if (radius < 1E-7 ) {
			//TODO
		}
		radii.push_back(radius);
	}

	Solid* pThis = node::ObjectWrap::Unwrap<Solid>(args.This());

	pThis->fillet(edges,radii);

	// size_t extractArgumentList(const Arguments& args,std::vector<ClassType*>& elements)
	return args.This();
}
Handle<v8::Value> Solid::chamfer(const v8::Arguments& args) 
{
	HandleScope scope;
	// [ <list of edges ], radius ]
	// [ <list of edges ], radius ]

	// size_t extractArgumentList(const Arguments& args,std::vector<ClassType*>& elements)
	return Handle<v8::Value>(Undefined());
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
      pThis->m_cacheMesh = Persistent<Object>::New(pThis->createMesh(0.001,3,true));
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
        Bnd_Box aBox;
        BRepBndLib::Add(shape, aBox);
        
        Standard_Real aXmin, aYmin, aZmin;
        Standard_Real aXmax, aYmax, aZmax;
        aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
        
        Standard_Real maxd = fabs(aXmax - aXmin);
        maxd = std::max(maxd, fabs(aYmax - aYmin));
        maxd = std::max(maxd, fabs(aZmax - aZmin));
        
        BRepMesh_FastDiscret MSH(factor*maxd, angle, aBox, Standard_True, Standard_True, 
                                 Standard_True, Standard_True);
        
        MSH.Perform(shape);
        
        if (shape.ShapeType() == TopAbs_COMPSOLID || shape.ShapeType() == TopAbs_COMPOUND) {
            TopExp_Explorer exSolid, exFace;
            for (exSolid.Init(shape, TopAbs_SOLID); exSolid.More(); exSolid.Next()) {
                const TopoDS_Solid& solid = static_cast<const TopoDS_Solid &>(exSolid.Current());
                for (exFace.Init(solid, TopAbs_FACE); exFace.More(); exFace.Next()) {
                    const TopoDS_Face& face = static_cast<const TopoDS_Face &>(exFace.Current());
                    if (face.IsNull()) continue;
                    mesh->extractFaceMesh(face, qualityNormals);
                }
            }
        }  else {
            TopExp_Explorer exFace;
            for (exFace.Init(shape, TopAbs_FACE); exFace.More(); exFace.Next()) {
                const TopoDS_Face& face = static_cast<const TopoDS_Face &>(exFace.Current());
                if (face.IsNull()) continue;
                mesh->extractFaceMesh(face, qualityNormals);
            }
        }
    } catch(Standard_Failure&) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        const Standard_CString msg = e->GetMessageString();
        if (msg != NULL && strlen(msg) > 1) {
            setErrorMessage(msg);
        } else {
            setErrorMessage("Failed to mesh object");
        }
        return  scope.Close(Object::New());
    }
    mesh->optimize();
    return scope.Close(theMesh);
}
