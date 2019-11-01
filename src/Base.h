#pragma once
#include "NodeV8.h"
#include "OCC.h"
#include "Util.h"
#include "vector"


class Base : public Nan::ObjectWrap {
public:
  int hashCode();
  bool isNull();
  bool isValid();
  const char* shapeType();
  const char* orientation();
  bool fixShape();

  virtual const TopoDS_Shape& shape() const = 0;
  virtual void setShape(const TopoDS_Shape&) = 0;
  virtual v8::Local<v8::Object>  Clone() const = 0;
  virtual Base* Unwrap(v8::Local<v8::Object> obj) const = 0;//  { return node::ObjectWrap::Unwrap<Edge>(obj); }

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

  static void  InitProto(v8::Local<v8::ObjectTemplate>& target);
};

v8::Local<v8::Object> buildEmptyWrapper(TopAbs_ShapeEnum type);
v8::Local<v8::Object> buildWrapper(const TopoDS_Shape shape);


template<class ClassType>
size_t extractArgumentList(_NAN_METHOD_ARGS, std::vector<ClassType*>& elements)
{
  elements.reserve(elements.size() + info.Length());

  for (int i = 0; i < info.Length(); i++) {
    if (IsInstanceOf<ClassType>(info[i])) {

      auto o = Nan::To<v8::Object>(info[i]).ToLocalChecked();
      
      elements.push_back(Nan::ObjectWrap::Unwrap<ClassType>(o));
    }
  }
  return elements.size();
}

template<class ClassType>
bool extractArg(const v8::Local<v8::Value>& value, ClassType*& pObj)
{
  assert(pObj == 0);
  if (value.IsEmpty()) return false;
  if (!value->IsObject()) return false;

  auto lo = Nan::To<v8::Object>(value).ToLocalChecked();
  if (IsInstanceOf<ClassType>(lo)) {
    pObj = Nan::ObjectWrap::Unwrap<ClassType>(lo);
    return true;
  }
  return false;
}


template<class ClassType>
bool _extractArray(const v8::Local<v8::Value>& value, std::vector<ClassType*>& elements)
{
  if (value->IsArray()) {
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(value);
    int length = arr->Length();
    elements.reserve(elements.size() + length);
    for (int i = 0; i < length; i++) {

      auto elementI = Nan::Get(arr,i).ToLocalChecked();
      if (!elementI->IsObject()) {
        return false; // element is not an object
      }
      v8::Local<v8::Object> obj = Nan::To<v8::Object>(elementI).ToLocalChecked();
      if (IsInstanceOf<ClassType>(obj)) {
        elements.push_back(Nan::ObjectWrap::Unwrap<ClassType>(obj));
      }
    }
  }
  else if (value->IsObject()) {
    // a single element
    v8::Local<v8::Object> obj = Nan::To<v8::Object>(value).ToLocalChecked();
    if (IsInstanceOf<ClassType>(obj)) {
      elements.push_back(Nan::ObjectWrap::Unwrap<ClassType>(obj));
    }
  }
  return elements.size() >= 1;
}






