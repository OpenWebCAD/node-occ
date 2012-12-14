#pragma once
#include "GeometryBuilder.h"
#include "Util.h"
#include "Transformation.h"

#include <map>
#include <vector>

using namespace std;






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




v8::Handle<v8::Value> Shape::translate(const v8::Arguments& args) 
{
  v8::HandleScope scope;
  Shape* pThis = ObjectWrap::Unwrap<Shape>(args.This());

  gp_Trsf transformation;

  double x=0,y=0,z=0;
  ReadPoint(args[0],&x,&y,&z);
  transformation.SetTranslation(gp_Vec(x,y,z));

  pThis->shape_ = BRepBuilderAPI_Transform(pThis->shape_, transformation).Shape();

  return scope.Close(args.This());

}

v8::Handle<v8::Value> Shape::rotate(const v8::Arguments& args) 
{
  v8::HandleScope scope;
  Shape* pThis = ObjectWrap::Unwrap<Shape>(args.This());

  return scope.Close(args.This());
}

v8::Handle<v8::Value>  Shape::applyTransform(const v8::Arguments& args)
{
  v8::HandleScope scope;
  Shape* pThis = ObjectWrap::Unwrap<Shape>(args.This());

  return scope.Close(args.This());
}

bool operator == (v8::Local<v8::String> str,const char* value)
{
  return str->Equals(v8::String::New(value));
}



//void Shape::ApplyWorkplane(v8::Handle<v8::Object> json) {
//	
//   v8::Local<v8::Object> workplane = json->Get(v8::String::New("workplane"))->ToObject();
//
//   if (!workplane->IsNull()) {
//        
//  
//        v8::Local<v8::Object> workplane_origin = workplane->Get(v8::String::New("origin"))->ToObject();
//        
//        double x = ReadDouble(workplane_origin,"x");
//        double y = ReadDouble(workplane_origin,"y");
//        double z = ReadDouble(workplane_origin,"z");
//
//        v8::Local<v8::Object> workplane_axis = workplane->Get(v8::String::New("axis"))->ToObject();  
//        
//        double u = ReadDouble(workplane_axis,"x");
//        double v = ReadDouble(workplane_axis,"y");
//        double w = ReadDouble(workplane_axis,"z");
//
//        double angle = ReadDouble(workplane,"angle");
//
//        gp_Trsf transformation1 = gp_Trsf();
//        transformation1.SetRotation(gp_Ax1(gp_Pnt(0.0,0.0,0.0), gp_Dir(u,v,w)), angle/180*M_PI);
//        shape_ = BRepBuilderAPI_Transform(shape_, transformation1).Shape();
//
//        gp_Trsf transformation2 = gp_Trsf();
//        transformation2.SetTranslation(gp_Vec(x,y,z));
//        shape_ = BRepBuilderAPI_Transform(shape_, transformation2).Shape();
//    }
//}
//
//void Shape::ApplyReverseWorkplane(v8::Handle<v8::Object> json) {
//	
//   v8::Local<v8::Object> workplane = json->Get(v8::String::New("workplane"))->ToObject();;
//   if (!workplane->IsNull()) {
//
//        v8::Local<v8::Object> workplane_origin = workplane->Get(v8::String::New("origin"))->ToObject();;
//        
//        double x = ReadDouble(workplane_origin,"x");
//        double y = ReadDouble(workplane_origin,"y");
//        double z = ReadDouble(workplane_origin,"z");
//
//        v8::Local<v8::Object> workplane_axis = workplane->Get(v8::String::New("axis"))->ToObject();;       
//        
//        double u = ReadDouble(workplane_axis,"x");
//        double v = ReadDouble(workplane_axis,"y");
//        double w = ReadDouble(workplane_axis,"z");
//
//        double angle = ReadDouble(workplane,"angle");
//        
//        gp_Trsf transformation1 = gp_Trsf();
//        transformation1.SetTranslation(gp_Vec(x,y,z));
//        shape_ = BRepBuilderAPI_Transform(shape_, transformation1).Shape();
//        
//        gp_Trsf transformation2 = gp_Trsf();
//        transformation2.SetRotation(gp_Ax1(gp_Pnt(0.0,0.0,0.0), gp_Dir(u,v,w)), angle/180*M_PI);
//        shape_ = BRepBuilderAPI_Transform(shape_, transformation2).Shape();
//        
//    }
//}




const TopoDS_Shape& Shape::shape()  const
{
    return shape_;
}

void Shape::setShape( const TopoDS_Shape& shape)
{
   shape_ = shape;
}