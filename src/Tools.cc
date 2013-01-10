#include "Tools.h"



#include "Shape.h"
#include "Solid.h"

#include <list>


void extractShapes(Local<Value> value,std::list<Shape*>& shapes)
{
    if (value->IsArray())  {

        Array* arr = Array::Cast(*value);
        for (uint32_t i=0; i<arr->Length(); i++) {
            extractShapes(arr->Get(i),shapes);
        }
    } else if (value->IsObject()) {
        // it must be of type
        Handle<Object> obj = value->ToObject();
        if (Solid::constructor->HasInstance(obj)) {
            shapes.push_back(node::ObjectWrap::Unwrap<Shape>(obj));
        }

    }
}




v8::Handle<Value> writeSTEP(const v8::Arguments& args)
{
    HandleScope scope;
    // first argument is filename
    if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("expecting a file name")));
        return scope.Close(Undefined());

    }
    v8::String::Utf8Value str(args[0]);
    const char* filename = ToCString(str);


    std::list<Shape*>  shapes;
    for (int i=1; i<args.Length(); i++) {
        extractShapes(args[i],shapes);
    }
    if (shapes.size()==0) {
        return scope.Close(Boolean::New(false));
    }


    try {
        STEPControl_Writer writer;
        IFSelect_ReturnStatus status;

        //xx Interface_Static::SetCVal("xstep.cascade.unit","M");
        //xx Interface_Static::SetIVal("read.step.nonmanifold", 1);

        for (std::list<Shape*>::iterator it = shapes.begin();it != shapes.end();it++) {
            status = writer.Transfer((*it)->shape(), STEPControl_AsIs);
            if (status != IFSelect_RetDone) {
                ThrowException(Exception::TypeError(String::New("Failed to write STEP file")));
            }
        }
        status = writer.Write(filename);
    } CATCH_AND_RETHROW("Failed to write STEP file ");
    return scope.Close(v8::True());
}




int extractSubShape(const TopoDS_Shape& shape, std::list<Local<Object> >& shapes)
{
    TopAbs_ShapeEnum type = shape.ShapeType();
    switch (type)
    {
    case TopAbs_COMPOUND:
        return 0;
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    {
		// shapes.push_back(Solid::NewInstance(shape)->ToObject());
        break;
    }
    case TopAbs_FACE:
    case TopAbs_SHELL:
    {
         break;
    }
    case TopAbs_WIRE:
    {
 
        break;
    }
    case TopAbs_EDGE:
    {
        break;
    }
    case TopAbs_VERTEX:
    {
        break;
    }
    default:
        return 0;
    }
    return 1;
}
   
int extractShape(const TopoDS_Shape& shape, std::list<Local<Object> >& shapes)
{
    TopAbs_ShapeEnum type = shape.ShapeType();
    
    if (type != TopAbs_COMPOUND) {
        extractSubShape(shape, shapes);
        return 0;
    }
    
    TopExp_Explorer ex;
    int ret = 0;
    
    // extract compund
    for (ex.Init(shape, TopAbs_COMPOUND); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    
    // extract solids
    for (ex.Init(shape, TopAbs_COMPSOLID); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    for (ex.Init(shape, TopAbs_SOLID); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    
    // extract free faces
    for (ex.Init(shape, TopAbs_SHELL, TopAbs_SOLID); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    for (ex.Init(shape, TopAbs_FACE, TopAbs_SOLID); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    
    // extract free wires
    for (ex.Init(shape, TopAbs_WIRE, TopAbs_FACE); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    
    // extract free edges
    for (ex.Init(shape, TopAbs_EDGE, TopAbs_WIRE); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
        
    // extract free vertices
    for (ex.Init(shape, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next())
        ret += extractSubShape(ex.Current(), shapes);
    
    return ret;
}

v8::Handle<Value> readSTEP(const v8::Arguments& args)
{
  HandleScope scope;
  if (!args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("expecting a file name")));
        return scope.Close(Undefined());

  }
  v8::String::Utf8Value str(args[0]);
  const char* filename = ToCString(str);

  try {
        STEPControl_Reader aReader;
        
        Interface_Static::SetCVal("xstep.cascade.unit","M");
        Interface_Static::SetIVal("read.step.nonmanifold", 1);
        
        if (aReader.ReadFile(filename) != IFSelect_RetDone) {
            StdFail_NotDone::Raise("Failed to read STEP file");
        }
        
        // Root transfers
        int nbr = aReader.NbRootsForTransfer();
        for (int n = 1; n<= nbr; n++) {
            aReader.TransferRoot(n);
        }
        
        // Collecting resulting entities
        int nbs = aReader.NbShapes();
        if (nbs == 0) {
			return scope.Close(Array::New());
		}
        
		
		std::list<Local<Object> > shapes;

        for (int i=1; i<=nbs; i++) {
            const TopoDS_Shape& aShape = aReader.Shape(i);
            extractShape(aShape, shapes);
        }
		
		Local<Array> arr = Array::New((int)shapes.size());
		int i=0;
		for (std::list<Local<Object> >::iterator it = shapes.begin();it != shapes.end();it++) {
			arr->Set(i,*it); 
			i++;
		}

		return scope.Close(Local<Value>(arr));

    } CATCH_AND_RETHROW("Failed to read STEP file ");
    return scope.Close(Undefined());
}