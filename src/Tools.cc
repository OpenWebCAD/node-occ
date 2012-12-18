#include "Tools.h"



#include "Shape.h"
#include "Solid.h"

#include <vector>


void extractShapes(Local<Value>& value,std::vector<Shape*>& shapes)
{
	if (value->IsArray())  {

		Array* arr = Array::Cast(*value);
		for (uint32_t i=0;i<arr->Length();i++) {
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
	}
    v8::String::Utf8Value str(args[0]);
    const char* filename = ToCString(str);


	std::vector<Shape*>  shapes;
	for (int i=1;i<args.Length();i++) {
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
        
        for (size_t i = 0; i < shapes.size(); i++) {
            status = writer.Transfer(shapes[i]->shape(), STEPControl_AsIs);
            if (status != IFSelect_RetDone) {
 				ThrowException(Exception::TypeError(String::New("Failed to write STEP file")));
            }
        }
        status = writer.Write(filename);
    } catch(Standard_Failure&) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        Standard_CString msg = e->GetMessageString();
        
        if (msg == NULL || strlen(msg) <= 1) {
            msg = "Failed to write STEP file";
        }
        ThrowException(Exception::TypeError(String::New(msg)));
        return Undefined();
    }
    return scope.Close(Boolean::New(true));
}
