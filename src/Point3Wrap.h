#pragma once
#include "NodeV8.h"
#include "OCC.h"

// expose a point
class Point3Wrap : public Nan::ObjectWrap {
  virtual const gp_XYZ get() const = 0;

  double x() {
    return get().X();
  }
  double y() {
    return get().Y();
  }
  double z() {
    return get().Z();
  }
public:
  // Methods exposed to JavaScript
  static NAN_METHOD(New);
  static NAN_METHOD(equals);
  static NAN_METHOD(asArray);
  static void Init(v8::Handle<v8::Object> target);
  static Nan::Persistent<v8::FunctionTemplate> _template;
};



#define TEAROFF_INIT(ACCESSOR)   m_##ACCESSOR(*this)

template <class _ThisType, class Wrapper, class CLASS, const CLASS(_ThisType::*ACCESSOR)() const>
class Accessor : public Wrapper {

  //    typedef Accessor<_ThisType, Wrapper,CLASS,ACCESSOR> THIS;
  _ThisType& m_parent;

public:

  Accessor(_ThisType& parent)
    :m_parent(parent) {
  }

  ~Accessor() {
  }

  virtual const CLASS get() const {
    return (m_parent.*ACCESSOR)();
  }

  static NAN_PROPERTY_GETTER(getter) {
    if (info.This().IsEmpty()) {
      info.GetReturnValue().SetUndefined();
      return;
    }
    if (info.This()->InternalFieldCount() == 0) {
      info.GetReturnValue().SetUndefined();
      return;
    }
    _ThisType* pThis = Nan::ObjectWrap::Unwrap<_ThisType>(info.This());

    info.GetReturnValue().Set(NewInstance(*pThis));

  }
  static v8::Handle<v8::Value> NewInstance(_ThisType& parent) {

    v8::Local<v8::Object> instance = Nan::New(Wrapper::_template)->GetFunction()->NewInstance(Nan::GetCurrentContext(),0, 0).ToLocalChecked();
    Accessor* pThis = new Accessor(parent);
    pThis->Wrap(instance);
    return instance;
  }
};



#define TEAROFF_POINT(THISTYPE,ACCESSOR,WRAPPER,CLASS)                         \
  typedef Accessor<THISTYPE,WRAPPER,CLASS,&THISTYPE::ACCESSOR> t##ACCESSOR;  \




#define EXPOSE_TEAROFF(THISTYPE,ACCESSOR)                                            \
  Nan::SetAccessor(proto,                                                            \
			Nan::New<v8::String>(#ACCESSOR).ToLocalChecked(),                        \
					&t##ACCESSOR::getter,  0,v8::Handle<v8::Value>(),v8::DEFAULT,(v8::PropertyAttribute)(v8::ReadOnly|v8::DontDelete))


#define REXPOSE_TEAROFF(THISTYPE,ACCESSOR)                                            \
    Nan::SetAccessor(info.This(),                                                 \
    Nan::New(#ACCESSOR).ToLocalChecked(),                                         \
     &t##ACCESSOR::getter,  0,v8::Handle<v8::Value>(),v8::DEFAULT,v8::ReadOnly)

