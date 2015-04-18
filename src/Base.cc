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
  NanScope();

  const Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

  try {

    gp_Trsf transformation;

    double x=0,y=0,z=0;

    ReadPoint(args[0],&x,&y,&z);
    transformation.SetTranslation(gp_Vec(x,y,z));

    Local<Object> copy    = pThis->Clone();

    pThis->Unwrap(copy)->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation,Standard_True).Shape());

    NanReturnValue(copy);

  } CATCH_AND_RETHROW("Failed to calculate a translated shape ");

  NanReturnUndefined();

}

NAN_METHOD(Base::rotate)
{
  NanScope();

  const Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

  try {

    gp_Trsf  transformation;

    ReadRotationFromArgs(args,transformation);

    Local<Object> copy    = pThis->Clone();
    pThis->Unwrap(copy)->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation,Standard_True).Shape());

    NanReturnValue(copy);

  } CATCH_AND_RETHROW("Failed to calculate a rotated shape ");

  NanReturnUndefined();
}

NAN_METHOD(Base::mirror)
{
  NanScope();

  const Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

  // TODO 
  Local<Object> copy    = pThis->Clone();
  NanReturnValue(copy);
}

NAN_METHOD(Base::applyTransform)
{
  NanScope();

  Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

  
  if (args.Length()!=1 && !NanHasInstance(Transformation::_template,args[0])) {
  //if (args.Length()!=1 && !Transformation::_template->HasInstance(args[0])) {
    NanThrowError("invalid  tansformation");
    NanReturnUndefined();
  }
  try {

    Transformation* pTrans =  ObjectWrap::Unwrap<Transformation>(args[0]->ToObject());

    const gp_Trsf& transformation = pTrans->m_trsf;
    pThis->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation).Shape());

    NanReturnValue(args.This());

  } CATCH_AND_RETHROW("Failed to calculate a transformed shape ");

  NanReturnUndefined();
}


NAN_METHOD(Base::transformed)
{
  NanScope();

  Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());

  if (args.Length()!=1 && !NanHasInstance(Transformation::_template,args[0])) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }


  try {
    Transformation* pTrans =  node::ObjectWrap::Unwrap<Transformation>(args[0]->ToObject());
    const gp_Trsf& trsf =  pTrans->m_trsf;
    gp_Trsf transformation =     trsf;

    Local<Object> copy    = pThis->Clone();

    if (!pThis->shape().IsNull()) {
      pThis->Unwrap(copy)->setShape(
        BRepBuilderAPI_Transform(pThis->shape(),
        transformation,Standard_True).Shape());
    }

    NanReturnValue(copy);
  } CATCH_AND_RETHROW("Failed to calculate a transformed shape ");

  NanReturnUndefined();

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
  NanScope();
  if (args.Length()!=0) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }
  Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());

  pThis->fixShape();

  NanReturnValue(args.This());
}

NAN_METHOD(Base::getBoundingBox)
{
  NanScope();

  if (args.Length()!=0) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }
  Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());

  try {

    const double tolerance= 1E-12;
    const TopoDS_Shape& shape = pThis->shape();
    Bnd_Box aBox;
    BRepBndLib::Add(shape, aBox);
    aBox.SetGap(tolerance);
    NanReturnValue(BoundingBox::NewInstance(aBox));

  } CATCH_AND_RETHROW("Failed to compute bounding box ");

  NanReturnUndefined();
}

NAN_METHOD(Base::clone)
{
  NanScope();

  if (args.Length()!=0) {
    NanThrowError("Wrong arguments");
    NanReturnUndefined();
  }
  Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());
  NanReturnValue(pThis->Clone());
}

void Base::InitNew(_NAN_METHOD_ARGS)
{
  REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,shapeType);
  REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,orientation);
}

void  Base::InitProto(Handle<ObjectTemplate>& proto)
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