#include "Solid.h"

#include "Util.h"
#include "Face.h"
#include "Edge.h"


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

  EXPOSE_METHOD(Solid,makeBox);
  EXPOSE_METHOD(Solid,makeCone);
  EXPOSE_METHOD(Solid,makeSphere);
  EXPOSE_METHOD(Solid,makeCylinder);

  EXPOSE_METHOD(Solid,fuse);
  EXPOSE_METHOD(Solid,cut);
  EXPOSE_METHOD(Solid,common);

  EXPOSE_METHOD(Solid,fillet);
  EXPOSE_METHOD(Solid,chamfer);

  EXPOSE_METHOD(Solid,getEdges);


  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numFaces);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Solid,numSolids);
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

Handle<v8::Value> Solid::NewInstance(const v8::Arguments& args) 
{
  HandleScope scope;

  const unsigned argc = 1;
  Handle<v8::Value> argv[argc] = { args[0] };
  Local<Object> instance = constructor->GetFunction()->NewInstance(argc, argv);

  return scope.Close(instance);
}





Handle<v8::Value> Solid::makeBox(const v8::Arguments& args) 
{

   // could be :
   //    3 numbers dx,dy,dz
   //    2 points  p1,p2
   //TODO   1 point + 3 numbers dx,dy,dz 
   //TODO    1 object with { x: 1,y: 2,z: 3, dw:
   Solid* pThis = ObjectWrap::Unwrap<Solid>(args.This());
   
   double dx = 10;
   double dy = 10;
   double dz = 10;
      
   try {
     if (args.Length() == 3 && args[0]->IsNumber() && args[1]->IsNumber()  && args[2]->IsNumber() ) {
        dx  = args[0]->ToNumber()->Value();
        dy  = args[1]->ToNumber()->Value();
        dz  = args[2]->ToNumber()->Value();
        pThis->shape_ = BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
     } else if (args.Length() == 2) {

       gp_Pnt p1;
       ReadPoint(args[0],&p1);

       gp_Pnt p2;
       ReadPoint(args[1],&p2);

       pThis->shape_ = BRepPrimAPI_MakeBox(p1,p2).Shape();

     } else if (args.Length() == 3) {

       gp_Pnt p1;
       ReadPoint(args[0],&p1);

       dx  = args[2]->ToNumber()->Value();
       dy  = args[3]->ToNumber()->Value();
       dz  = args[4]->ToNumber()->Value();
     
       pThis->shape_ = BRepPrimAPI_MakeBox(p1,dx, dy, dz).Shape();


     } 
   }
   catch(Standard_Failure&) {
      pThis->shape_.Nullify();
   }
   // xx pThis->PostProcess(json);
   return args.This();

}


Handle<Value> Solid::makePrism(const Arguments& args)
{
	HandleScope scope;
    // can work with this
	Handle<Object> pJhis = args.This();
	if ( pJhis.IsEmpty() || !Solid::constructor->HasInstance(pJhis))  {
		// create a new fapce;
		pJhis = constructor->GetFunction()->CallAsConstructor(0,0)->ToObject();
	}					 						
	Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

	// <FACE> [x,y,z]
	// <FACE> [x,y,z] [x,y,z]
	// OCCBase *shape, OCCStruct3d p1, OCCStruct3d p2) 
	Face* pFace = DynamicCast<Face>(args[0]);

	gp_Vec direction(0,0,10);
	ReadVector(args[1],&direction);

	if (!pFace || direction.IsEqual(gp_Vec(0,0,0),1E-7,1E-8)) {
		ThrowException(Exception::TypeError(String::New("invalid arguments : expecting <FACE>,<VECTOR>")));
	}																

    try {		   
        const TopoDS_Shape& face = pFace->face();
        //// Only accept Edge or Wire
        //TopAbs_ShapeEnum type = shp.ShapeType();
        //if (type != TopAbs_EDGE && type != TopAbs_WIRE) {
        //    StdFail_NotDone::Raise("expected Edge or Wire");
        //}
       
        BRepPrimAPI_MakePrism prismMaker(face, direction);
		
		//xx prismMaker.Check();

        pThis->setShape(prismMaker.Shape());
        
        // possible fix shape
        if (!pThis->fixShape())	 {
            StdFail_NotDone::Raise("Shapes not valid");
		}
        
    } CATCH_AND_RETHROW("Failed to create prism ");

    return scope.Close(pJhis);
}


Handle<Value> Solid::makeSphere(const Arguments& args)
{
	HandleScope scope;
    // can work with this
	Handle<Object> pJhis = args.This();
	if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
		// create a new object
		pJhis = constructor->GetFunction()->CallAsConstructor(0,0)->ToObject();
	}					 						
	Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);

	gp_Pnt center(0,0,0);
	ReadPoint(args[0],&center);
	double radius = args[1]->ToNumber()->Value();
	if (radius < 1E-7) {
	   ThrowException(Exception::Error(String::New("invalid radius")));
	    return scope.Close(pJhis);
	}
    try {
       
        pThis->setShape(BRepPrimAPI_MakeSphere(center, radius).Shape());

    } CATCH_AND_RETHROW("Failed to create sphere ");

    return scope.Close(pJhis);
}

bool ReadAx2(Handle<Value>& value,gp_Ax2* ax2)
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
	} CATCH_AND_RETHROW("Failed to extract position");
	return false;
}
Handle<Value> Solid::makeCylinder(const Arguments& args)
{
	HandleScope scope;
    // can work with this
	Handle<Object> pJhis = args.This();
	if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
		// create a new object
		pJhis = constructor->GetFunction()->CallAsConstructor(0,0)->ToObject();
	}					 		
	Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);


	//  gp_Ax2& Axes
	//  gp_Ax2(const gp_Pnt& P,const gp_Dir& N,const gp_Dir& Vx);
 	// Standard_EXPORT   BRepPrimAPI_MakeCylinder(const Standard_Real R,const Standard_Real H);
	// Standard_EXPORT   BRepPrimAPI_MakeCylinder(const Standard_Real R,const Standard_Real H,const Standard_Real Angle);
    // Standard_EXPORT   BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,const Standard_Real R,const Standard_Real H);
	// Standard_EXPORT   BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,const Standard_Real R,const Standard_Real H,const Standard_Real Angle);
    const double epsilon = 1E-3;

	gp_Ax2 axis;
 	if (args.Length()==2) {

		// variation 1	 <R:number> <H:number> 
		// a vertical cylinder of radius R starting a (0,0,0) ending at (0,0,H)
		double R  = args[0]->ToNumber()->Value();
		double H  = args[1]->ToNumber()->Value();

		if ( R < epsilon || H < epsilon ) {
			ThrowException(Exception::Error(String::New("invalid value for arguments")));
		}
		try {
			pThis->setShape(BRepPrimAPI_MakeCylinder(R,H).Shape());
		} CATCH_AND_RETHROW("Failed to create cylinder ");

	} else if (args.Length()==3) {

		if (args[0]->IsArray() && args[1]->IsNumber() && args[2]->IsNumber()) {
			// variation 2
			//  <[ <Origin[x,yz]>, <VZn[x,yz]>,<VXn[x,yz]>] <R:number> <H:number>
			gp_Ax2  ax2;
			bool success = ReadAx2(args[0],&ax2);

 			double R  = args[1]->ToNumber()->Value();
			double H  = args[2]->ToNumber()->Value();

			if ( R < epsilon || H < epsilon ) {
				ThrowException(Exception::Error(String::New("invalid value for arguments")));
			}
			try {
				pThis->setShape(BRepPrimAPI_MakeCylinder(ax2,R,H).Shape());
			} CATCH_AND_RETHROW("Failed to create cylinder ");

		} else if (args[0]->IsArray() && args[1]->IsArray() && args[2]->IsNumber()) {
			// variation 3
			gp_Pnt p1;
			ReadPoint(args[0],&p1);
			
			gp_Pnt p2;
			ReadPoint(args[1],&p1);

			double R  = args[2]->ToNumber()->Value();
			      
			const double dx = p2.X() - p1.X();
			const double dy = p2.Y() - p1.Y();
			const double dz = p2.Z() - p1.Z();

			const double H = sqrt(dx*dx + dy*dy + dz*dz);
			if (H < epsilon ) {
				ThrowException(Exception::Error(String::New("cannot build a cylinder on two coincident points")));
			}
			gp_Vec aV(dx / H, dy / H, dz / H);
			gp_Ax2 ax2(p1, aV);
			try {
				pThis->setShape(BRepPrimAPI_MakeCylinder(ax2,R,H).Shape());
			} CATCH_AND_RETHROW("Failed to create cylinder ");
			
		}
	} else {
		ThrowException(Exception::Error(String::New("invalid arguments")));
	}

    return scope.Close(pJhis);
}


Handle<Value> Solid::makeCone(const Arguments& args)
{							
	HandleScope scope;
    // can work with this
	Handle<Object> pJhis = args.This();
	if ( pJhis.IsEmpty() || !constructor->HasInstance(pJhis))  {
		// create a new object
		pJhis = constructor->GetFunction()->CallAsConstructor(0,0)->ToObject();
	}					 						
	Solid* pThis = node::ObjectWrap::Unwrap<Solid>(pJhis);



    const double epsilon = 1E-3;


    // Standard_EXPORT   BRepPrimAPI_MakeCone(const Standard_Real R1,const Standard_Real R2,const Standard_Real H);
	// Standard_EXPORT   BRepPrimAPI_MakeCone(const Standard_Real R1,const Standard_Real R2,const Standard_Real H,const Standard_Real angle);
	// Standard_EXPORT   BRepPrimAPI_MakeCone(const gp_Ax2& Axes,const Standard_Real R1,const Standard_Real R2,const Standard_Real H,const Standard_Real angle);
	if (args.Length()==3) {
		double R1 = args[0]->ToNumber()->Value();
		double R2 = args[1]->ToNumber()->Value();
		double H  = args[2]->ToNumber()->Value();

		if ( R1 < epsilon || R2 < epsilon || H < epsilon ) {
			ThrowException(Exception::Error(String::New("invalid value for arguments")));
		}
		try {
			pThis->setShape(BRepPrimAPI_MakeCone(R1, R2,H).Shape());
		} CATCH_AND_RETHROW("Failed to create sphere ");
	}  else {
		ThrowException(Exception::Error(String::New("invalid arguments")));
	}

    return scope.Close(pJhis);
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


typedef enum BoolOpType {
  BOOL_FUSE,
  BOOL_CUT,
  BOOL_COMMON,
} BoolOpType;

int Solid::boolean(Solid *tool, BoolOpType op)
{
    try {
        TopoDS_Shape shape;
        switch (op) {
            case BOOL_FUSE:
            {
                BRepAlgoAPI_Fuse FU (tool->shape(), this->shape());
                if (!FU.IsDone())
                    Standard_ConstructionError::Raise("operation failed");
                shape = FU.Shape();
                break;
            }
            case BOOL_CUT:
            {
                BRepAlgoAPI_Cut CU (this->shape(), tool->shape());
                if (!CU.IsDone())
                    Standard_ConstructionError::Raise("operation failed");
                shape = CU.Shape();
                break;
            }
            case BOOL_COMMON:
            {
                BRepAlgoAPI_Common CO (tool->shape(), this->shape());
                if (!CO.IsDone())
                    Standard_ConstructionError::Raise("operation failed");
                shape = CO.Shape();
                break;
            }
            default:
                Standard_ConstructionError::Raise("unknown operation");
                break;
        }
        
        // check for empty compund shape
        TopoDS_Iterator It (shape, Standard_True, Standard_True);
        int found = 0;
        for (; It.More(); It.Next())
            found++;
        if (found == 0) {
            Standard_ConstructionError::Raise("result object is empty compound");
        }
        
        this->setShape(shape);
        
        // possible fix shape
        if (!this->fixShape())
            StdFail_NotDone::Raise("Shapes not valid");
        
    } catch(Standard_Failure &) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        const Standard_CString msg = e->GetMessageString();
        if (msg != NULL && strlen(msg) > 1) {
            setErrorMessage(msg);
        } else {
            setErrorMessage("Failed in boolean operation");
        }
        return 0;
    }
    return 1;
}


Handle<v8::Value> Solid::_boolean(const v8::Arguments& args,BoolOpType op) 
{
  HandleScope scope;
  
  Local<v8::Value> v = args[0];
  if (v.IsEmpty() || !v->IsObject()) {
      ThrowException(Exception::TypeError(String::New("Wrong v8::Arguments")));
      return scope.Close(Undefined());
  }
  Solid* pThis = ObjectWrap::Unwrap<Solid>(args.This());
  Solid* pTool = ObjectWrap::Unwrap<Solid>(v->ToObject());

  int ret =  pThis->boolean(pTool, op);

  return scope.Close(args.This());

}
Handle<v8::Value> Solid::fuse(const v8::Arguments& args) 
{
  return _boolean(args,BOOL_FUSE);
}
Handle<v8::Value> Solid::cut(const v8::Arguments& args) 
{
  return _boolean(args,BOOL_CUT);
}
Handle<v8::Value> Solid::common(const v8::Arguments& args) 
{
  return _boolean(args,BOOL_COMMON);
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
      pThis->m_cacheMesh = Persistent<Object>::New(pThis->createMesh(1.0,15.0,true));
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