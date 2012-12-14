#include "Solid.h"

#include "Util.h"


 //v8::Handle<v8::Value> GetPointCoord(v8::Local<v8::String> property,const v8::AccessorInfo &info)
 //{
 //  
 //
 //   Solid* obj = node::ObjectWrap::Unwrap<Solid>(info.This());

 //   double value = rand();
 //   if (property->Equals(v8::String::New("x"))) {
 //      value = rand();
 //   }
 //   if (property->Equals(v8::String::New("y"))) {
 //      value = rand();
 //   }
 //   if (property->Equals(v8::String::New("z"))) {
 //      value = rand();
 //   }
 //   return v8::Number::New(value);
 // }
 // 
 // void noset(v8::Local<v8::String> property, v8::Local<v8::Value> value,const v8::AccessorInfo& info) 
 // {
 //  //C Local<Object> self = info.Holder();
 //  //C Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
 //   //C void* ptr = wrap->Value();
 //   //C static_cast<Point*>(ptr)->x_ = value->Int32Value();
 //  Solid* obj = node::ObjectWrap::Unwrap<Solid>(info.This());


 // }

 v8::Persistent<v8::Function> Solid::constructor;



   
 
 /*static*/void Solid::Init(v8::Handle<v8::Object> target)
 {
  srand ( time(NULL) );


  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(Solid::New);
  tpl->SetClassName(v8::String::NewSymbol("Solid"));

  // object has one internal filed ( the C++ object)
  tpl->InstanceTemplate()->SetInternalFieldCount(1);


  // Prototype
  v8::Local<v8::ObjectTemplate> objTemplate = tpl->PrototypeTemplate();


  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("translate"),v8::FunctionTemplate::New(translate)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("rotate"),   v8::FunctionTemplate::New(rotate)->GetFunction());  
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("makeBox"),  v8::FunctionTemplate::New(makeBox)->GetFunction());

  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("fuse"),  v8::FunctionTemplate::New(fuse)->GetFunction());

  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("isNull"), 
    ee<Shape,v8::Boolean,bool,&Shape::isNull>,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("isValid"), 
    ee<Shape,v8::Boolean,bool,&Shape::isValid>, 0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);

  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("numFaces"), 
    ee<Solid,v8::Integer,int,&Solid::numFaces>,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);
  
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("numSolids"), 
    ee<Solid,v8::Integer,int,&Solid::numSolids>,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);

  //XxCtpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("location"), 
  //XxC  ee<Shape,v8::Object,v8::Object,&Shape::location>, 0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);

  //tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("x"), GetPointCoord, noset);
  //tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("y"), GetPointCoord, noset);
  //tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("z"), GetPointCoord, noset);

  //tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("width"),  GetPointCoord, noset);
  //tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("depth"),  GetPointCoord, noset);
  //tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("height"), GetPointCoord, noset);


  constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  target->Set(v8::String::NewSymbol("Solid"), constructor);

}

v8::Handle<v8::Value> Solid::New(const v8::Arguments& args)
{
  v8::HandleScope scope;
  
  Solid* obj = new Solid();
  obj->Wrap(args.This());
  // return scope.Close(args.This());
  return args.This();
}

v8::Handle<v8::Value> Solid::NewInstance(const v8::Arguments& args) 
{
  v8::HandleScope scope;

  const unsigned argc = 1;
  v8::Handle<v8::Value> argv[argc] = { args[0] };
  v8::Local<v8::Object> instance = constructor->NewInstance(argc, argv);

  return scope.Close(instance);
}



void ReadPoint(v8::Local<v8::Value>& value,gp_Pnt* pt)
{
  double x=0,y=0,z=0;
  ReadPoint(value,&x,&y,&z);
  pt->SetCoord(x,y,z);
}

v8::Handle<v8::Value> Solid::makeBox(const v8::Arguments& args) 
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

void Solid::Mesh() 
{
    TopExp_Explorer Ex; 
    int numFaces = 0;
    for (Ex.Init(shape_, TopAbs_FACE); Ex.More(); Ex.Next()) { 
        ++numFaces;
    }
    
    if (numFaces > 0) {
        BRepMesh().Mesh(shape_, 1.0);
    }
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
//
//double Solid::area() {
//    GProp_GProps prop;
//    BRepGProp::SurfaceProperties(this->getSolid(), prop);
//    return prop.Mass();
//}
//
//double Solid::volume() {
//    GProp_GProps prop;
//    BRepGProp::VolumeProperties(this->getSolid(), prop);
//    return prop.Mass();
//}

//DVec Solid::inertia() {
//    DVec ret;
//    GProp_GProps prop;
//    BRepGProp::VolumeProperties(this->getSolid(), prop);
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
        
    } catch(Standard_Failure &err) {
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
v8::Handle<v8::Value> Solid::fuse(const v8::Arguments& args) 
{
  v8::HandleScope scope;
  
  v8::Local<v8::Value> v = args[0];
  if (v.IsEmpty() || !v->IsObject()) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
      return scope.Close(v8::Undefined());
  }
  Solid* pThis = ObjectWrap::Unwrap<Solid>(args.This());
  Solid* pTool = ObjectWrap::Unwrap<Solid>(v->ToObject());

  int ret =  pThis->boolean(pTool, Solid::BOOL_FUSE);

  return scope.Close(args.This());

}