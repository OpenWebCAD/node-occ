#include "ShapeIterator.h"
#include "Solid.h"
#include "Face.h"
#include "Wire.h"
#include "Edge.h"
#include "Vertex.h"
#include "Shell.h"


Local<Object> buildEmptyWrapper(TopAbs_ShapeEnum type)
{
    switch(type) {
    case  TopAbs_COMPOUND:
    case  TopAbs_COMPSOLID:
    case  TopAbs_SOLID:
        return NanNew(Solid::_template)->GetFunction()->NewInstance(0,0)->ToObject();
    case TopAbs_SHELL:
        return NanNew(Shell::_template)->GetFunction()->NewInstance(0,0)->ToObject();;
        break;
    case TopAbs_FACE:
        return NanNew(Face::_template)->GetFunction()->NewInstance(0,0)->ToObject();
    case TopAbs_WIRE:
        return NanNew(Wire::_template)->GetFunction()->NewInstance(0,0)->ToObject();
    case TopAbs_EDGE:
        return NanNew(Edge::_template)->GetFunction()->CallAsConstructor(0,0)->ToObject();
    case TopAbs_VERTEX:
        return NanNew(Vertex::_template)->GetFunction()->NewInstance(0,0)->ToObject();
    case TopAbs_SHAPE:
        break;
    }
    return Local<Object>();
}
Local<Object> buildWrapper(const TopoDS_Shape shape)
{
    
    Local<Object> obj = Local<Object>(buildEmptyWrapper(shape.ShapeType()));
    Base*  pShape = node::ObjectWrap::Unwrap<Base>(obj);
    pShape->setShape(shape);
    return obj;
}

bool ShapeIterator::more()
{
    return ex.More() ? true : false;
}

Handle<Value> ShapeIterator::next()
{
    if (ex.More()) {

        Local<Object>  obj = buildWrapper(ex.Current());

        NanObjectWrapHandle(this)->Set(NanNew("current"),obj);

        ex.Next();

        return obj;
    } else {
        return NanUndefined();
    }
}

NAN_METHOD(ShapeIterator::next)
{
    NanScope();
    if(!NanHasInstance(ShapeIterator::_template,args.This())) {
        NanThrowError("bad arguments");
        NanReturnUndefined();
    }
    ShapeIterator* pThis = ObjectWrap::Unwrap<ShapeIterator>(args.This());
    
    NanReturnValue(pThis->next());
}


void ShapeIterator::reset()
{
    ex.ReInit();
}


NAN_METHOD(ShapeIterator::reset)
{
    NanScope();

    if(NanHasInstance(ShapeIterator::_template,args.This())) {
        NanThrowError("bad arguments");
        NanReturnUndefined();
    }
    ShapeIterator* pThis = ObjectWrap::Unwrap<ShapeIterator>(args.This());

    pThis->reset();

    NanReturnValue(args.This());
}

Persistent<FunctionTemplate> ShapeIterator::_template;

void ShapeIterator::Init(Handle<Object> target)
{

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(ShapeIterator::New);  
    tpl->SetClassName(NanNew("ShapeIterator"));

    // object has one internal filed ( the C++ object)
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NanAssignPersistent<v8::FunctionTemplate>(_template, tpl);

    // Prototype
    Local<ObjectTemplate> proto = tpl->PrototypeTemplate();

    Base::InitProto(proto);


    EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ShapeIterator,more);

    EXPOSE_METHOD(ShapeIterator,next);
    EXPOSE_METHOD(ShapeIterator,reset);


    target->Set(NanNew("ShapeIterator"), tpl->GetFunction());

}


TopAbs_ShapeEnum getShapeEnum(const Handle<Value> arg)
{
    if (arg->IsString()) {

        Handle<String> str = arg->ToString();
        if (str->Equals(NanNew("COMPOUND"))) {
            return TopAbs_COMPOUND;
        }
        if (str->Equals(NanNew("COMPSOLID"))) {
            return TopAbs_COMPSOLID;
        }
        if (str->Equals(NanNew("VERTEX")))   {
            return TopAbs_VERTEX;
        }
        if (str->Equals(NanNew("EDGE")))     {
            return TopAbs_EDGE;
        }
        if (str->Equals(NanNew("WIRE")))     {
            return TopAbs_WIRE;
        }
        if (str->Equals(NanNew("FACE")))   {
            return TopAbs_FACE;
        }
        if (str->Equals(NanNew("SHELL")))    {
            return TopAbs_SHELL;
        }
        if (str->Equals(NanNew("SOLID")))    {
            return TopAbs_SOLID;
        }
    }
    return TopAbs_SHAPE;
};

NAN_METHOD(ShapeIterator::New)
{
    NanScope();

    if (!args.IsConstructCall()) {
        NanThrowError(" use new occ.ShapeIterator() to construct a ShapeIterator");
        NanReturnUndefined();
    }

    if (args.Length() != 2) {
        NanThrowError(" expecting two arguments : <Shape>,<'VERTEX'|'WIRE'|'SOLID'|'FACE'...>");
        NanReturnUndefined();
    }

    // TODO (check that the object args[0] has the correct type)
    Base* pShape = node::ObjectWrap::Unwrap<Base>(args[0]->ToObject());

    TopAbs_ShapeEnum type = getShapeEnum(args[1]);

    ShapeIterator* pThis = new ShapeIterator(pShape,type);

    args.This()->Set(NanNew("current"),NanUndefined());

    pThis->Wrap(args.This());

    NanReturnValue(args.This());
}

