#pragma once
#include "GeometryBuilder.h"
#include "Util.h"
#include "Transform.h"

#include <map>
#include <vector>

using namespace std;



 v8::Handle<v8::Value> GetPointCoord(v8::Local<v8::String> property,const v8::AccessorInfo &info)
 {
   
 
    Solid* obj = node::ObjectWrap::Unwrap<Solid>(info.This());

    double value = rand();
    if (property->Equals(v8::String::New("x"))) {
       value = rand();
    }
    if (property->Equals(v8::String::New("y"))) {
       value = rand();
    }
    if (property->Equals(v8::String::New("z"))) {
       value = rand();
    }
    return v8::Number::New(value);
  }
  
  void noset(v8::Local<v8::String> property, v8::Local<v8::Value> value,const v8::AccessorInfo& info) 
  {
   //C Local<Object> self = info.Holder();
   //C Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    //C void* ptr = wrap->Value();
    //C static_cast<Point*>(ptr)->x_ = value->Int32Value();
   Solid* obj = node::ObjectWrap::Unwrap<Solid>(info.This());


  }


bool Shape::isNull() 
{
    return shape_.IsNull() ? true : false;
}

bool Shape::isValid()
{
    if (shape_.IsNull())
        return false;
    BRepCheck_Analyzer aChecker(shape_);
    return aChecker.IsValid() ? true : false;
}



 v8::Persistent<v8::Function> Solid::constructor;


 template<class T,typename T1,typename T2,typename T2 (T::*func)() >
 v8::Handle<v8::Value> ee(v8::Local<v8::String> property,const v8::AccessorInfo &info)
 {
    T* obj = node::ObjectWrap::Unwrap<T>(info.This());
    return T1::New((obj->*func)());

 }
   
 
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

  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("isNull"), 
    ee<Shape,v8::Boolean,bool,&Shape::isNull>,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("isValid"), 
    ee<Shape,v8::Boolean,bool,&Shape::isValid>, 0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);
 
  //XxCtpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("location"), 
  //XxC  ee<Shape,v8::Object,v8::Object,&Shape::location>, 0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly);

  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("x"), GetPointCoord, noset);
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("y"), GetPointCoord, noset);
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("z"), GetPointCoord, noset);

  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("width"),  GetPointCoord, noset);
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("depth"),  GetPointCoord, noset);
  tpl->PrototypeTemplate()->SetAccessor(v8::String::NewSymbol("height"), GetPointCoord, noset);


  Solid::constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
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
   catch(Standard_Failure &err) {
      pThis->shape_.Nullify();
   }
   // xx pThis->PostProcess(json);
   return args.This();

}


v8::Handle<v8::Value> Shape::translate(const v8::Arguments& args) 
{
  v8::HandleScope scope;
  Shape* obj = ObjectWrap::Unwrap<Shape>(args.This());
  return scope.Close(args.This());
}

v8::Handle<v8::Value> Shape::rotate(const v8::Arguments& args) 
{
  v8::HandleScope scope;
  Shape* obj = ObjectWrap::Unwrap<Shape>(args.This());
  return scope.Close(args.This());
}

void Shape::ApplyOrigin(v8::Handle<v8::Object> json) 
{

    v8::Local<v8::Object> origin = json->Get(v8::String::New("origin"))->ToObject();

    if (!origin->IsUndefined()) {  
      double x=0,y=0,z=0;
      ReadXYZ(origin,&x,&y,&z);
      gp_Trsf transformation = gp_Trsf();
      transformation.SetTranslation(gp_Vec(x,y,z));        
      shape_ = BRepBuilderAPI_Transform(shape_, transformation).Shape();
    }
}

bool operator == (v8::Local<v8::String> str,const char* value)
{
  return str->Equals(v8::String::New(value));
}

void Shape::ApplyTransform(v8::Handle<v8::Object> transformJson)
 {
	
    v8::Local<v8::String> transformType = transformJson->Get(v8::String::New("type"))->ToString(); 

    v8::Local<v8::Object> origin = transformJson->Get(v8::String::New("origin"))->ToObject();
    v8::Local<v8::Object> parameters = transformJson->Get(v8::String::New("parameters"))->ToObject();

    
    if (transformType->Equals(v8::String::New("rotate"))) {
        auto_ptr< Transformer<RotateTransform> > transformer(new Transformer<RotateTransform>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if (transformType == "scale") {
        auto_ptr< Transformer<Scale> > transformer(new Transformer<Scale>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if (transformType == "translate") {
        auto_ptr< Transformer<TranslateTransform> > transformer(new Transformer<TranslateTransform>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if ((transformType == "mirror") || (transformType == "axis_mirror")) {
        auto_ptr< Transformer<AxisMirror> > transformer(new Transformer<AxisMirror>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } else if (transformType == "plane_mirror") {
        auto_ptr< Transformer<PlaneMirror> > transformer(new Transformer<PlaneMirror>(shape_, origin, parameters));
        shape_ = transformer->transformed_shape();
        return;
    } 
    throw "transform type not found";
}


void Shape::ApplyTransforms(v8::Handle<v8::Object> json) {
	
    v8::Local<v8::Value> transforms = json->Get(v8::String::New("transforms"));

    if (transforms->IsArray()) {

          v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(transforms);
          int length = array->Length();

      
      
        for ( int k = 0; k < length; ++k) {

            v8::Local<v8::Value> transformJson = array->Get(k);

            if (transformJson->IsObject()) {
              this->ApplyTransform(transformJson->ToObject());
            }
        }
    } 
}


void Shape::ApplyWorkplane(v8::Handle<v8::Object> json) {
	
   v8::Local<v8::Object> workplane = json->Get(v8::String::New("workplane"))->ToObject();

   if (!workplane->IsNull()) {
        
  
        v8::Local<v8::Object> workplane_origin = workplane->Get(v8::String::New("origin"))->ToObject();
        
        double x = ReadDouble(workplane_origin,"x");
        double y = ReadDouble(workplane_origin,"y");
        double z = ReadDouble(workplane_origin,"z");

        v8::Local<v8::Object> workplane_axis = workplane->Get(v8::String::New("axis"))->ToObject();  
        
        double u = ReadDouble(workplane_axis,"x");
        double v = ReadDouble(workplane_axis,"y");
        double w = ReadDouble(workplane_axis,"z");

        double angle = ReadDouble(workplane,"angle");

        gp_Trsf transformation1 = gp_Trsf();
        transformation1.SetRotation(gp_Ax1(gp_Pnt(0.0,0.0,0.0), gp_Dir(u,v,w)), angle/180*M_PI);
        shape_ = BRepBuilderAPI_Transform(shape_, transformation1).Shape();

        gp_Trsf transformation2 = gp_Trsf();
        transformation2.SetTranslation(gp_Vec(x,y,z));
        shape_ = BRepBuilderAPI_Transform(shape_, transformation2).Shape();
    }
}

void Shape::ApplyReverseWorkplane(v8::Handle<v8::Object> json) {
	
   v8::Local<v8::Object> workplane = json->Get(v8::String::New("workplane"))->ToObject();;
   if (!workplane->IsNull()) {

        v8::Local<v8::Object> workplane_origin = workplane->Get(v8::String::New("origin"))->ToObject();;
        
        double x = ReadDouble(workplane_origin,"x");
        double y = ReadDouble(workplane_origin,"y");
        double z = ReadDouble(workplane_origin,"z");

        v8::Local<v8::Object> workplane_axis = workplane->Get(v8::String::New("axis"))->ToObject();;       
        
        double u = ReadDouble(workplane_axis,"x");
        double v = ReadDouble(workplane_axis,"y");
        double w = ReadDouble(workplane_axis,"z");

        double angle = ReadDouble(workplane,"angle");
        
        gp_Trsf transformation1 = gp_Trsf();
        transformation1.SetTranslation(gp_Vec(x,y,z));
        shape_ = BRepBuilderAPI_Transform(shape_, transformation1).Shape();
        
        gp_Trsf transformation2 = gp_Trsf();
        transformation2.SetRotation(gp_Ax1(gp_Pnt(0.0,0.0,0.0), gp_Dir(u,v,w)), angle/180*M_PI);
        shape_ = BRepBuilderAPI_Transform(shape_, transformation2).Shape();
        
    }
}




TopoDS_Shape Shape::shape() 
{
    return shape_;
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

void Solid::PostProcess(v8::Handle<v8::Object> json)
{
    this->ApplyOrigin(json);
    this->ApplyTransforms(json);
    this->ApplyWorkplane(json);
    this->Mesh();
}
