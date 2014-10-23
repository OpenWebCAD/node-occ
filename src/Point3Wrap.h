#pragma once
#include "NodeV8.h"
#include "OCC.h"

// expose a point
class Point3Wrap : public node::ObjectWrap {
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
  // Methods exposed to JavaScripts
  static NAN_METHOD(New);
  static void Init(v8::Handle<v8::Object> target);  
  static v8::Persistent<v8::FunctionTemplate> _template;
};



#define TEAROFF_INIT(ACCESSOR)   m_##ACCESSOR(*this)

template <class _ThisType, class Wrapper,class CLASS,const CLASS (_ThisType::*ACCESSOR)() const>
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
    NanScope();
    if (args.This().IsEmpty()) {
       NanReturnUndefined();
    }
    if (args.This()->InternalFieldCount() == 0 ) {
       NanReturnUndefined();
    }
    _ThisType* pThis = node::ObjectWrap::Unwrap<_ThisType>(args.This());

    NanReturnValue(NewInstance(*pThis));

  }
  static Handle<Value> NewInstance(_ThisType& parent) {
   
    Local<Object> instance = NanNew(Wrapper::_template)->GetFunction()->NewInstance(0,0);
    Accessor* pThis = new Accessor(parent);
    pThis->Wrap(instance);
    return instance;
  }
};



#define TEAROFF_POINT(THISTYPE,ACCESSOR,WRAPPER,CLASS)                         \
  typedef Accessor<THISTYPE,WRAPPER,CLASS,&THISTYPE::ACCESSOR> t##ACCESSOR;  \




#define EXPOSE_TEAROFF(THISTYPE,ACCESSOR)                           \
  proto->SetAccessor(NanNew<v8::String>(#ACCESSOR), &t##ACCESSOR::getter,  0,Handle<v8::Value>(),DEFAULT,(PropertyAttribute)(ReadOnly|DontDelete))
