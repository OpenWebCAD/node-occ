#include "Base.h"
#include "Util.h"
#include "BoundingBox.h"
#include "Transformation.h"


Base::~Base()
{
}

bool Base::isNull()
{
  return shape().IsNull() ? true : false;
}

bool Base::isValid()
{
  if (isNull()) {
    return false;
  }
  BRepCheck_Analyzer aChecker(shape());
  return aChecker.IsValid() ? true : false;
}

const char* Base::shapeType()
{
  if (isNull()) {
    return "UNDEFINED";
  }
  switch(shape().ShapeType()) {
  case    TopAbs_COMPOUND:
    return "COMPOUND";
    break;
  case    TopAbs_COMPSOLID:
    return "COMPSOLID";
    break;
  case    TopAbs_SOLID:
    return "SOLID";
    break;
  case    TopAbs_SHELL:
    return "SHELL";
    break;
  case    TopAbs_FACE:
    return "FACE";
    break;
  case    TopAbs_WIRE:
    return "WIRE";
    break;
  case    TopAbs_EDGE:
    return "EDGE";
    break;
  case    TopAbs_VERTEX:
    return "VERTEX";
    break;
  case    TopAbs_SHAPE:
    return "SHAPE";
    break;
  }
  assert(0=="invalid case");
  return "???";
}

const char* Base::orientation()
{
  if (isNull()) {
    return "UNDEFINED";
  }
  TopAbs_Orientation value = shape().Orientation();
  switch(value) {
  case TopAbs_FORWARD:
    return "FORWARD";
    break;
  case TopAbs_REVERSED:
    return "REVERSED";
    break;
  case TopAbs_INTERNAL:
    return "INTERNAL";
    break;
  case TopAbs_EXTERNAL:
    return "EXTERNAL";
    break;
  }
  return "???";
}

NAN_METHOD(Base::translate)
{

  CHECK_THIS_DEFINED(Base);
  const Base* pThis = ObjectWrap::Unwrap<Base>(info.This());

  try {

    gp_Trsf transformation;

    double x=0,y=0,z=0;
  if (info.Length() == 3) {
    ReadDouble(info[0], x);
    ReadDouble(info[1], y);
    ReadDouble(info[2], z);
  }
  else if (info.Length() == 1) {
    ReadPoint(info[0], &x, &y, &z);
  }
  else {
    return Nan::ThrowError("Wrong Arguments");
  }
    transformation.SetTranslation(gp_Vec(x,y,z));

    v8::Local<v8::Object> copy    = pThis->Clone();

    pThis->Unwrap(copy)->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation,Standard_True).Shape());

    return info.GetReturnValue().Set(copy);

  } CATCH_AND_RETHROW("Failed to calculate a translated shape ");

}

NAN_METHOD(Base::rotate)
{
  CHECK_THIS_DEFINED(Base);
  const Base* pThis = ObjectWrap::Unwrap<Base>(info.This());

  try {

    gp_Trsf  transformation;

    ReadRotationFromArgs(info,transformation);

    v8::Local<v8::Object> copy    = pThis->Clone();
    pThis->Unwrap(copy)->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation,Standard_True).Shape());

    return info.GetReturnValue().Set(copy);

  } CATCH_AND_RETHROW("Failed to calculate a rotated shape ");

}

NAN_METHOD(Base::mirror)
{
  CHECK_THIS_DEFINED(Base);
  const Base* pThis = ObjectWrap::Unwrap<Base>(info.This());
  // TODO 
  v8::Local<v8::Object> copy    = pThis->Clone();
  info.GetReturnValue().Set(copy);
}

NAN_METHOD(Base::applyTransform)
{

  CHECK_THIS_DEFINED(Base);
  Base* pThis = ObjectWrap::Unwrap<Base>(info.This());

  if (info.Length()!=1 && !IsInstanceOf<Transformation>(info[0]) ) {
    return Nan::ThrowError("invalid  transformation");
  }
  try {

    Transformation* pTrans =  ObjectWrap::Unwrap<Transformation>(Nan::To<v8::Object>(info[0]).ToLocalChecked());

    const gp_Trsf& transformation = pTrans->m_trsf;
    pThis->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation).Shape());

	info.GetReturnValue().Set(info.This());

  } CATCH_AND_RETHROW("Failed to calculate a transformed shape ");

}


NAN_METHOD(Base::transformed)
{
  CHECK_THIS_DEFINED(Base);
  Base* pThis = Nan::ObjectWrap::Unwrap<Base>(info.This());

  if (info.Length()!=1) {
    return Nan::ThrowError("Wrong arguments");
  }
  Transformation* pTrans = DynamicCast<Transformation>(info[0]);
  if (!pTrans) {
    return Nan::ThrowError("transform expects a Transformation object");
  }

  try {
    const gp_Trsf& trsf =  pTrans->m_trsf;
    gp_Trsf transformation =     trsf;

    v8::Local<v8::Object> copy    = pThis->Clone();

    if (!pThis->shape().IsNull()) {
      pThis->Unwrap(copy)->setShape(
        BRepBuilderAPI_Transform(pThis->shape(),
        transformation,Standard_True).Shape());
    }

	info.GetReturnValue().Set(copy);
  } CATCH_AND_RETHROW("Failed to calculate a transformed shape ");

}


//void Shape::ApplyWorkplane(Handle<Object> json) {
//
//   Local<Object> workplane = json->Get(String::New("workplane"))->ToObject();
//
//   if (!workplane->IsNull()) {
//
//
//        Local<Object> workplane_origin = workplane->Get(String::New("origin"))->ToObject();
//
//        double x = ReadDouble(workplane_origin,"x");
//        double y = ReadDouble(workplane_origin,"y");
//        double z = ReadDouble(workplane_origin,"z");
//
//        Local<Object> workplane_axis = workplane->Get(String::New("axis"))->ToObject();
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
//void Shape::ApplyReverseWorkplane(Handle<Object> json) {
//
//   Local<Object> workplane = json->Get(String::New("workplane"))->ToObject();;
//   if (!workplane->IsNull()) {
//
//        Local<Object> workplane_origin = workplane->Get(String::New("origin"))->ToObject();;
//
//        double x = ReadDouble(workplane_origin,"x");
//        double y = ReadDouble(workplane_origin,"y");
//        double z = ReadDouble(workplane_origin,"z");
//
//        Local<Object> workplane_axis = workplane->Get(String::New("axis"))->ToObject();;
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
bool Base::fixShape()
{

  if (this->shape().IsNull()) {
    return false;
  }
  BRepCheck_Analyzer aChecker(this->shape());
  if (!aChecker.IsValid()) {
    ShapeFix_ShapeTolerance aSFT;
    aSFT.LimitTolerance(this->shape(),Precision::Confusion(),Precision::Confusion());

    occHandle(ShapeFix_Shape) aSfs = new ShapeFix_Shape(this->shape());
    aSfs->SetPrecision(Precision::Confusion());
    aSfs->Perform();
    const TopoDS_Shape aShape = aSfs->Shape();
    aChecker.Init(aShape, Standard_False);

    if (aChecker.IsValid() /* && this->canSetShape(aShape) */) {
      this->setShape(aShape);
    }
  }
  return aChecker.IsValid() ? true : false;

}

NAN_METHOD(Base::fixShape)
{
  CHECK_THIS_DEFINED(Base);
  Base* pThis = Nan::ObjectWrap::Unwrap<Base>(info.This());
  if (info.Length()!=0) {
    return Nan::ThrowError("Wrong arguments");
  }

  pThis->fixShape();

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Base::getBoundingBox)
{

  CHECK_THIS_DEFINED(Base);
  Base* pThis = Nan::ObjectWrap::Unwrap<Base>(info.This());
  if (info.Length()!=0) {
    return Nan::ThrowError("Wrong arguments");
  }

  try {

    const double tolerance= 1E-12;
    const TopoDS_Shape& shape = pThis->shape();
    Bnd_Box aBox;
    BRepBndLib::Add(shape, aBox);
    aBox.SetGap(tolerance);
	info.GetReturnValue().Set(BoundingBox::NewInstance(aBox));

  } CATCH_AND_RETHROW("Failed to compute bounding box ");

}

NAN_METHOD(Base::clone)
{
  CHECK_THIS_DEFINED(Base);
  Base* pThis = Nan::ObjectWrap::Unwrap<Base>(info.This());

  if (info.Length()!=0) {
    return Nan::ThrowError("Wrong arguments");
  }
  info.GetReturnValue().Set(pThis->Clone());
}

void Base::InitNew(_NAN_METHOD_ARGS)
{
  REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,shapeType);
  REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,orientation);
}

void  Base::InitProto(v8::Local<v8::ObjectTemplate>& proto)
{
  EXPOSE_METHOD(Base,clone);
  EXPOSE_METHOD(Base,translate);
  EXPOSE_METHOD(Base,rotate);
  EXPOSE_METHOD(Base,mirror);

  EXPOSE_METHOD(Base,transformed);
  EXPOSE_METHOD(Base,applyTransform);
  EXPOSE_METHOD(Base,getBoundingBox);

  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Base,isNull);
  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(Base,isValid);
  EXPOSE_READ_ONLY_PROPERTY_INTEGER(Base,hashCode);
  EXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,shapeType);
  EXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,orientation);

}

int Base::hashCode()
{
  return shape().HashCode(std::numeric_limits<int>::max());
}
