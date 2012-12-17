#include "Solid.h"

#include "Util.h"


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
  EXPOSE_METHOD(Solid,fuse);
  EXPOSE_METHOD(Solid,cut);
  EXPOSE_METHOD(Solid,common);



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
    // return FromConstructorTemplate(constructor, args);
  }
  
  Solid* obj = new Solid();
  obj->Wrap(args.This());
  // return scope.Close(args.This());
  return args.This();
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