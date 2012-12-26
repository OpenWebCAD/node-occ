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
		return Solid::constructor->GetFunction()->NewInstance(0,0)->ToObject();
	case TopAbs_SHELL:
		return Shell::constructor->GetFunction()->NewInstance(0,0)->ToObject();;
		break;
	case TopAbs_FACE:
		return Face::constructor->GetFunction()->NewInstance(0,0)->ToObject();
	case TopAbs_WIRE:
		return Wire::constructor->GetFunction()->NewInstance(0,0)->ToObject();
	case TopAbs_EDGE:
		return Edge::constructor->GetFunction()->CallAsConstructor(0,0)->ToObject();
	case TopAbs_VERTEX:
		return Vertex::constructor->GetFunction()->NewInstance(0,0)->ToObject();
	case TopAbs_SHAPE:
		break;
	}
	return Local<Object>();
}
Local<Object> buildWrapper(const TopoDS_Shape shape)
{								
	HandleScope scope;
	Local<Object> obj = Local<Object>(buildEmptyWrapper(shape.ShapeType()));
	Base*  pShape = node::ObjectWrap::Unwrap<Base>(obj);		
	pShape->setShape(shape);
	return scope.Close(obj);
}

bool ShapeIterator::more()
{
	return ex.More() ? true : false;
}

Handle<Value> ShapeIterator::next() 
{
	HandleScope scope;

	if (ex.More()) {

		Local<Object>  obj = buildWrapper(ex.Current());

		this->handle_->Set(String::NewSymbol("current"),obj);

		ex.Next();

		return scope.Close(obj);
	} else {
		return scope.Close(Undefined());
	}
}

Handle<v8::Value> ShapeIterator::next(const v8::Arguments& args)
{
	HandleScope scope;
	if(!ShapeIterator::constructor->HasInstance(args.This())) {
      ThrowException(Exception::TypeError(String::New("bad arguments")));   
	  return scope.Close(Undefined());
	}
	ShapeIterator* pThis = ObjectWrap::Unwrap<ShapeIterator>(args.This());	
	return scope.Close(pThis->next());
}


void ShapeIterator::reset() 
{
    ex.ReInit();
}
Handle<v8::Value> ShapeIterator::reset(const v8::Arguments& args)
{
	HandleScope scope;
	if(ShapeIterator::constructor->HasInstance(args.This())) {
      ThrowException(Exception::TypeError(String::New("bad arguments")));  
	   return scope.Close(Undefined());
	}
	ShapeIterator* pThis = ObjectWrap::Unwrap<ShapeIterator>(args.This());	

	pThis->reset();

	return args.This();
}

Persistent<FunctionTemplate> ShapeIterator::constructor;
void ShapeIterator::Init(Handle<Object> target)
{

  // Prepare constructor template
  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(ShapeIterator::New));
  constructor->SetClassName(String::NewSymbol("ShapeIterator"));

  // object has one internal filed ( the C++ object)
  constructor->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Local<ObjectTemplate> proto = constructor->PrototypeTemplate();

  Base::InitProto(proto);


  EXPOSE_READ_ONLY_PROPERTY_BOOLEAN(ShapeIterator,more);

  EXPOSE_METHOD(ShapeIterator,next);
  EXPOSE_METHOD(ShapeIterator,reset);

  

  target->Set(String::NewSymbol("ShapeIterator"), constructor->GetFunction());

}		 


TopAbs_ShapeEnum getShapeEnum(const Handle<Value> arg)
{
	if (arg->IsString()) {

		Handle<String> str = arg->ToString();
		if (str->Equals(String::NewSymbol("COMPOUND"))) { return TopAbs_COMPOUND;}
		if (str->Equals(String::NewSymbol("COMPSOLID"))){ return TopAbs_COMPSOLID;}
		if (str->Equals(String::NewSymbol("VERTEX")))   { return TopAbs_VERTEX;}
		if (str->Equals(String::NewSymbol("EDGE")))     { return TopAbs_EDGE;}
		if (str->Equals(String::NewSymbol("WIRE")))     { return TopAbs_WIRE;}
		if (str->Equals(String::NewSymbol("FACE")))   { return TopAbs_FACE;}
		if (str->Equals(String::NewSymbol("SHELL")))    { return TopAbs_SHELL;}
		if (str->Equals(String::NewSymbol("SOLID")))    { return TopAbs_SOLID;}
	}
	return TopAbs_SHAPE;
};			   

Handle<v8::Value> ShapeIterator::New(const v8::Arguments& args)
{
  HandleScope scope;
  if (!args.IsConstructCall()) {
	ThrowException(Exception::TypeError(String::New(" use new occ.ShapeIterator() to construct a ShapeIterator")));
	return scope.Close(Undefined());
  }					
  if (args.Length() != 2) {
	ThrowException(Exception::TypeError(String::New(" expecting two arguments : <Shape>,<'VERTEX'|'WIRE'|'SOLID'|'FACE'...>")));
	return scope.Close(Undefined());
  }
  // TODO (check that the object args[0] has the correct type)

  Base* pShape = node::ObjectWrap::Unwrap<Base>(args[0]->ToObject());
  
  TopAbs_ShapeEnum type = getShapeEnum(args[1]);
  
  ShapeIterator* pThis = new ShapeIterator(pShape,type);

  args.This()->Set(String::NewSymbol("current"),Undefined());

  pThis->Wrap(args.This());
  
  return args.This();
}

