#pragma once
#include "Base.h"
#include "NodeV8.h"
#include "OCC.h"

class Shape : public Base {

public:
  Shape(){};
  virtual ~Shape(){};

protected:
  TopoDS_Shape shape_;

  // static NAN_METHOD(ShapeType);
  // constructors

public:
  virtual const TopoDS_Shape &shape() const;
  virtual void setShape(const TopoDS_Shape &);

  void setErrorMessage(const char *message){};
};

template <class SHAPE>
inline v8::Local<v8::Array>
extract_shapes_as_javascript_array(SHAPE *pThis, TopAbs_ShapeEnum type) {

  Nan::EscapableHandleScope scope;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(pThis->shape(), type, anIndices);

  int nb = anIndices.Extent();
  v8::Local<v8::Array> arr = Nan::New<v8::Array>(nb);
  for (int i = 0; i < nb; i++) {
    v8::Local<v8::Object> obj = buildWrapper(anIndices(i + 1)); // 1 based !!!
    Nan::Set(arr, i, obj);
  }
  return scope.Escape(arr);
}
