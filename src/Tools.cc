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

static bool extractFileName(const Handle<Value>& value,std::string& filename)
{
    // first argument is filename
    if (!value->IsString()) {
      return false;
    }
    v8::String::Utf8Value str(value);
    filename = ToCString(str);
    return true;
}
static bool extractCallback(const Handle<Value>& value, Handle<Function>& callback)
{
   if(!value->IsFunction()) {
     return false;
   }
   callback = Handle<Function>::Cast(value->ToObject());
   assert(!callback.IsEmpty());
   return true;
}

v8::Handle<Value> writeSTEP(const v8::Arguments& args)
{
    HandleScope scope;
    std::string filename;
    if (!extractFileName(args[0],filename)) {
        ThrowException(Exception::TypeError(String::New("expecting a file name")));
        return scope.Close(Undefined());
    }

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
        status = writer.Write(filename.c_str());
    } CATCH_AND_RETHROW("Failed to write STEP file ");
    return scope.Close(v8::True());
}

v8::Handle<Value> writeBREP(const v8::Arguments& args)
{
    HandleScope scope;
    std::string filename;
    if (!extractFileName(args[0],filename)) {
      ThrowException(Exception::TypeError(String::New("expecting a file name")));
      return scope.Close(Undefined());
    }
    std::list<Shape*>  shapes;
    for (int i=1; i<args.Length(); i++) {
      extractShapes(args[i],shapes);
    }
    if (shapes.size()==0) {
      return scope.Close(Boolean::New(false));
    }

    try {
        BRep_Builder B;
        TopoDS_Compound C;
        B.MakeCompound(C);
        for (std::list<Shape*>::iterator it = shapes.begin();it != shapes.end();it++) {
            TopoDS_Shape shape = (*it)->shape();
            B.Add(C,shape);
        }
        BRepTools::Write(C, filename.c_str());
    } CATCH_AND_RETHROW("Failed to write BREP file ");
    return scope.Close(v8::True());
}



static int extractSubShape(const TopoDS_Shape& shape, std::list<Local<Object> >& shapes)
{
    TopAbs_ShapeEnum type = shape.ShapeType();
    switch (type)
    {
    case TopAbs_COMPOUND:
        return 0;
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    {
        shapes.push_back(Solid::NewInstance(shape)->ToObject());
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
   
static int extractShape(const TopoDS_Shape& shape, std::list<Local<Object> >& shapes)
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

static Local<Array> convert(std::list<Local<Object> > & shapes) {
  Local<Array> arr = Array::New((int)shapes.size());
  int i=0;
  for (std::list<Local<Object> >::iterator it = shapes.begin();it != shapes.end();it++) {
    arr->Set(i,*it); 
    i++;
  }
  return arr;
}

v8::Handle<Value> readSTEP(const v8::Arguments& args)
{
    HandleScope scope;
    std::string filename;
    if (!extractFileName(args[0],filename)) {
      ThrowException(Exception::TypeError(String::New("expecting a file name")));
      return scope.Close(Undefined());
    }
    v8::Local<Function> callback;
    if (!extractCallback(args[1],callback)) {
      return ThrowException(Exception::TypeError(String::New("expecting a callback function")));
    }

    Local<Object> global = v8::Context::GetCurrent()->Global();


    try {
        STEPControl_Reader aReader;
        
        Interface_Static::SetCVal("xstep.cascade.unit","M");
        Interface_Static::SetIVal("read.step.nonmanifold", 1);
        
        if (aReader.ReadFile(filename.c_str()) != IFSelect_RetDone) {
             Local<Value> argv[] = { Local<Value>(String::New(" cannot read STEP file"))  };
             Local<Value>  res =  callback->Call(global, 1, argv);
            return scope.Close(Undefined());
        }
        
        // Root transfers
        int nbr = aReader.NbRootsForTransfer();
        for (int n = 1; n<= nbr; n++) {
            aReader.TransferRoot(n);
        }
        
        // Collecting resulting entities
        int nbs = aReader.NbShapes();

        
        std::list<Local<Object> > shapes;
        for (int i=1; i<=nbs; i++) {
            const TopoDS_Shape& aShape = aReader.Shape(i);
            extractShape(aShape, shapes);
        }


        Local<Array> arr = convert(shapes); 
        Local<Value> err = Integer::New(0);
        Local<Value> argv[2] = { err, arr };
        Local<Value>  res =  callback->Call(global, 2, argv);



    } CATCH_AND_RETHROW("Failed to read STEP file ");
    return scope.Close(Undefined());
}

v8::Handle<Value> readBREP(const v8::Arguments& args)
{
    HandleScope scope;
    std::string filename;
    if (!extractFileName(args[0],filename)) {
      return ThrowException(Exception::TypeError(String::New("expecting a file name")));
    }
    v8::Local<Function> callback;
    if (!extractCallback(args[1],callback)) {
      return ThrowException(Exception::TypeError(String::New("expecting a callback function")));
    }

    Local<Object> global = v8::Context::GetCurrent()->Global();

    try {
        // read brep-file
        TopoDS_Shape shape;
        BRep_Builder aBuilder;
        if (!BRepTools::Read(shape, filename.c_str(), aBuilder)) {
             Local<Value> argv[] = { Local<Value>(String::New(" cannot read BREP file"))  };
             Local<Value>  res =  callback->Call(global, 1, argv);
             return scope.Close(Undefined());
        }
        std::list<Local<Object> > shapes;
        extractShape(shape, shapes);
        Local<Array> arr = convert(shapes); 
        Local<Value> err = Integer::New(0);
        Local<Value> argv[2] = { err, arr };
        Local<Value>  res =  callback->Call(global, 2, argv);

    } CATCH_AND_RETHROW("Failed to read BREP file");

    
    return scope.Close(Undefined());

}
