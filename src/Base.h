#pragma once
#include "NodeV8.h"
#include "OCC.h"



class Base : public node::ObjectWrap 
{
public:
   int hashCode(); 
   bool isNull();
   bool isValid();
   const char* shapeType(); 

   virtual const TopoDS_Shape& shape() const = 0;
   virtual void setShape(const TopoDS_Shape& ) = 0;
public:
// Methods exposed to JavaScripts
  static Handle<Value> translate(const Arguments& args);
  static Handle<Value> rotate(const Arguments& args);
  static Handle<Value> scale(const Arguments& args);
  static Handle<Value> mirror(const Arguments& args);
  static Handle<Value> applyTransform(const Arguments& args);

  static void  InitProto(Handle<ObjectTemplate> target);
};
