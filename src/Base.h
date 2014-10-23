#pragma once
#include "NodeV8.h"
#include "OCC.h"

#include "vector"


class Base : public node::ObjectWrap {
public:
  int hashCode();
  bool isNull();
  bool isValid();
  const char* shapeType();
  const char* orientation();
  bool fixShape();

  virtual const TopoDS_Shape& shape() const = 0;
  virtual void setShape(const TopoDS_Shape& ) = 0;
  virtual Local<Object>  Clone() const =0;
  virtual Base* Unwrap(v8::Local<v8::Object> obj) const =0;//  { return node::ObjectWrap::Unwrap<Edge>(obj); }

  virtual ~Base();

  virtual void InitNew(_NAN_METHOD_ARGS);

public:
  // Methods exposed to JavaScripts
  static NAN_METHOD(translate);
  static NAN_METHOD(rotate);
  static NAN_METHOD(scale);
  static NAN_METHOD(mirror);
  static NAN_METHOD(applyTransform);
  static NAN_METHOD(transformed);
  static NAN_METHOD(fixShape);
  static NAN_METHOD(clone);
  static NAN_METHOD(getBoundingBox);

  static void  InitProto(Handle<ObjectTemplate>& target);
};

v8::Local<v8::Object> buildEmptyWrapper(TopAbs_ShapeEnum type);
v8::Local<v8::Object> buildWrapper(const TopoDS_Shape shape);

#define CATCH_AND_RETHROW(message)                                    \
  catch(Standard_Failure& ) {                                        \
  Handle_Standard_Failure e = Standard_Failure::Caught();       \
  Standard_CString msg = e->GetMessageString();                 \
  if (msg == NULL || strlen(msg) < 1) {                         \
  msg = message;                                            \
  }                                                             \
  NanThrowError(msg);           \
}                                                                 \

template<class ClassType>
size_t extractArgumentList(_NAN_METHOD_ARGS,std::vector<ClassType*>& elements)
{
  elements.reserve(elements.size()+args.Length());

  for (int i=0; i<args.Length(); i++) {
    if (NanHasInstance(ClassType::_template,args[i]->ToObject())) {
      elements.push_back(node::ObjectWrap::Unwrap<ClassType>(args[i]->ToObject()));
    }
  }
  return elements.size();
}

template<class ClassType> 
bool extractArg(const Local<Value>& value,ClassType*& pObj)
{
  assert(pObj==0);
  if ( value.IsEmpty()) return false;
  if ( !value->IsObject()) return false;

  if (NanHasInstance(ClassType::_template,value->ToObject())) {
    pObj = node::ObjectWrap::Unwrap<ClassType>(value->ToObject());
    return true;
  }
  return false;
}


template<class ClassType>
bool _extractArray(const Handle<Value>& value,std::vector<ClassType*>& elements)
{
  if (value->IsArray())  {
    Handle<Array> arr = Handle<Array>::Cast(value);
    int length = arr->Length();
    elements.reserve(elements.size()+length);
    for (int i=0; i<length; i++) {
      if (!arr->Get(i)->IsObject()) {
        return false; // element is not an object
      }
      Handle<Object> obj = arr->Get(i)->ToObject();
      if (NanHasInstance(ClassType::_template,obj)) {
        elements.push_back(node::ObjectWrap::Unwrap<ClassType>(obj));
      }
    }
  } else if (value->IsObject())  {
    // a single element
    Handle<Object> obj = value->ToObject();
    if (NanHasInstance(ClassType::_template,obj)) {
      elements.push_back(node::ObjectWrap::Unwrap<ClassType>(obj));
    }
  }
  return elements.size() >=1;
}





template <typename OBJECT>
OBJECT* DynamicCast(const Handle<Value>& value)
{
  if (value.IsEmpty()) return 0;
  if (!value->IsObject()) return 0;
  if (NanHasInstance(OBJECT::_template,value->ToObject())) {
    return node::ObjectWrap::Unwrap<OBJECT>(value->ToObject());
  }
  return 0;
}
template <typename ObjType1,typename ObjType2>
Base*  DynamicCast(const Handle<Value>& value)
{
  ObjType1* obj = DynamicCast<ObjType1>(value);
  if (obj) return obj;
  return DynamicCast<ObjType2>(value);
}
