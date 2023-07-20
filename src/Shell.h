#pragma once
#include "NodeV8.h"
#include "OCC.h"

#include "Base.h"

#include <vector>
class Face;
class Shell : public Base {

  TopoDS_Shell m_shell;

public:
  int numFaces();

  double area();

  virtual const TopoDS_Shape &shape() const;
  const TopoDS_Shell &shell() const { return m_shell; }
  virtual void setShape(const TopoDS_Shape &);

  virtual Base *Unwrap(v8::Local<v8::Object> obj) const {
    return Nan::ObjectWrap::Unwrap<Shell>(obj);
  }
  virtual v8::Local<v8::Object> Clone() const;

  static NAN_METHOD(New);
  static NAN_MODULE_INIT(Init);
  static Nan::Persistent<v8::FunctionTemplate> _template;
};