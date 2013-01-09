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

Handle<Value> Base::translate(const Arguments& args)
{
    HandleScope scope;
    Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

    gp_Trsf transformation;

    double x=0,y=0,z=0;

    ReadPoint(args[0],&x,&y,&z);
    transformation.SetTranslation(gp_Vec(x,y,z));

    pThis->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation).Shape());

    return scope.Close(args.This());

}

Handle<Value> Base::rotate(const Arguments& args)
{
    HandleScope scope;
    Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

    gp_Trsf  transformation;
    ReadRotationFromArgs(args,transformation);

    pThis->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation).Shape());

    return scope.Close(args.This());
}
Handle<Value> Base::mirror(const Arguments& args)
{
    HandleScope scope;
    Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

    return scope.Close(args.This());
}

Handle<Value>  Base::applyTransform(const Arguments& args)
{
    HandleScope scope;
    Base* pThis = ObjectWrap::Unwrap<Base>(args.This());

    if (args.Length()!=1 && !Transformation::constructor->HasInstance(args[0])) {
        ThrowException(Exception::Error(String::New("invalid  tansformation")));
        return scope.Close(Undefined());
    }

    Transformation* pTrans =  ObjectWrap::Unwrap<Transformation>(args[0]->ToObject());

    const gp_Trsf& transformation = pTrans->m_trsf;
    pThis->setShape(BRepBuilderAPI_Transform(pThis->shape(), transformation).Shape());

    return scope.Close(args.This());
}


Handle<Value> Base::transformed(const Arguments& args)
{
    HandleScope scope;
    if (args.Length()!=1 && !Transformation::constructor->HasInstance(args[0])) {
        ThrowException(Exception::TypeError(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }

    Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());

    Transformation* pTrans =  node::ObjectWrap::Unwrap<Transformation>(args[0]->ToObject());
    const gp_Trsf& trsf =  pTrans->m_trsf;
    gp_Trsf transformation =     trsf;

    Local<Object> copy    = pThis->Clone();

    if (!pThis->shape().IsNull()) {
        pThis->Unwrap(copy)->setShape(
            BRepBuilderAPI_Transform(pThis->shape(),
                                     transformation,Standard_True).Shape());
    }

    return scope.Close(copy);

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

Handle<Value> Base::fixShape(const Arguments& args)
{
    HandleScope scope;
    if (args.Length()!=0) {
        ThrowException(Exception::Error(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());

    pThis->fixShape();

    return scope.Close(args.This());
}

Handle<v8::Value> Base::getBoundingBox(const v8::Arguments& args)
{
    HandleScope scope;

    if (args.Length()!=0) {
        ThrowException(Exception::Error(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());

    const double tolerance= 1E-12;
    const TopoDS_Shape& shape = pThis->shape();
    Bnd_Box aBox;
    BRepBndLib::Add(shape, aBox);
    aBox.SetGap(tolerance);

    return scope.Close(BoundingBox::NewInstance(aBox));
}


Handle<Value> Base::clone(const Arguments& args)
{
    HandleScope scope;

    if (args.Length()!=0) {
        ThrowException(Exception::Error(String::New("Wrong arguments")));
        return scope.Close(Undefined());
    }
    Base* pThis = node::ObjectWrap::Unwrap<Base>(args.This());
	return scope.Close(pThis->Clone());
}

void Base::InitNew(const v8::Arguments& args)
{
	REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,shapeType);
	REXPOSE_READ_ONLY_PROPERTY_CONST_STRING(Base,orientation);
}

void  Base::InitProto(Handle<ObjectTemplate> proto)
{
    EXPOSE_METHOD(Base,clone);
    EXPOSE_METHOD(Base,translate);
    EXPOSE_METHOD(Base,rotate);
    EXPOSE_METHOD(Base,mirror);

    EXPOSE_METHOD(Base,transformed);
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