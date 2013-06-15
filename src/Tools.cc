#include "Tools.h"



#include "Shape.h"
#include "Solid.h"

#include <list>
#include <strstream>

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
		return ThrowException(Exception::TypeError(String::New("expecting a file name")));
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
				return ThrowException(Exception::TypeError(String::New("Failed to write STEP file")));
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
		return ThrowException(Exception::TypeError(String::New("expecting a file name")));
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



// #undef Handle
// #define   Handle(ClassName)      Handle_##ClassName
// DEFINE_STANDARD_HANDLE(MyProgressIndicator, Message_ProgressIndicator)
class MyProgressIndicator : public Message_ProgressIndicator
{
	uv_async_t& m_async;
	double     m_lastValue;
public:
MyProgressIndicator(uv_async_t& async);
virtual Standard_Boolean Show(const Standard_Boolean force);
// DEFINE_STANDARD_RTTI(MyProgressIndicator);
};
// IMPLEMENT_STANDARD_RTTIEXT(MyProgressIndicator,Message_ProgressIndicator);

typedef struct data_s data_t;
struct data_s  {
	uv_work_t req;
	std::string filename;
	std::string message;
	uv_async_t async;
	Persistent<Function> callback;
	Persistent<Function> progressCallback;
	std::list<TopoDS_Shape > shapes;
	int retValue;
	double progress;
};


MyProgressIndicator::MyProgressIndicator(uv_async_t& async)
:Message_ProgressIndicator(),m_async(async),m_lastValue(0)
{
}
Standard_Boolean MyProgressIndicator::Show(const Standard_Boolean force)
{
	data_t* data = (data_t *)this->m_async.data;
	double value = this->GetPosition();
	double delta = (value-this->m_lastValue);
	if ( delta > 0.05) {
		data->progress = int(delta*1000);// this->GetPosition();
		this->m_lastValue = value;
		uv_async_send(&m_async);
	}
    return Standard_False;
}

bool mutex_initialised = false;
uv_mutex_t stepOperation_mutex= {0};
class MutexLocker
{
	uv_mutex_t& m_mutex;
public:
	MutexLocker(uv_mutex_t& mutex) 
		:m_mutex(mutex)
	{
		uv_mutex_lock(&m_mutex);
	}
	~MutexLocker() 
	{
		uv_mutex_unlock(&m_mutex);
	}
};

void notify_progress(uv_async_t* handle,int status/*unused*/) 
{
	data_t* data = (data_t *)handle->data;
	if (!data->progressCallback.IsEmpty()) {
		// std::cout << " progress received" <<  data->progress << " \n";
		Local<Value> argv[1] = {Integer::New(data->progress) };
		Local<Value>  res =  data->progressCallback->Call(Context::GetCurrent()->Global(), 1, argv);
	}
}

void _readStepAsync(uv_work_t *req) {

	MutexLocker _locker(stepOperation_mutex);
  

	data_t* data = (data_t *) req->data;
	uv_async_init(req->loop,&data->async,notify_progress);
	data->async.data = data;

	data->retValue = 0;
	std::string filename = data->filename;

	occHandle(Message_ProgressIndicator) progress = new MyProgressIndicator(data->async) ;
	progress->SetScale(1,100,1);
	progress->Show();    


	try {
        
		STEPControl_Reader aReader;
        
		Interface_Static::SetCVal("xstep.cascade.unit","mm");
		Interface_Static::SetIVal("read.step.nonmanifold", 1);
		
		progress->NewScope(5,"reading");
		if (aReader.ReadFile(filename.c_str()) != IFSelect_RetDone) {
			std::strstream str;
			str << " cannot read STEP file " << filename << std::ends;
			data->message = str.str();
			// Local<Value> argv[] = { Local<Value>(String::New())  };
			//  Local<Value>  res =  callback->Call(global, 1, argv);
			// return scope.Close(Undefined());
			progress->EndScope();
			data->retValue = 1;
			return;
		}
		progress->EndScope();
		progress->Show();       


		progress->NewScope(95,"transfert");
		progress->Show();        
		aReader.WS()->MapReader()->SetProgress(progress);

		// Root transfers
		int nbr = aReader.NbRootsForTransfer();
		progress->SetRange(0,nbr);
		int mod = nbr/10+1;
		for (int n = 1; n<= nbr; n++) {
            
			aReader.TransferRoot(n);
			
			if ((n+1)%mod==0) { progress->Increment(); }
		}     
		
		aReader.WS()->MapReader()->SetProgress(0);
		progress->EndScope();
		progress->Show(); 

		int nbs = aReader.NbShapes();
		for (int i=1; i<=nbs; i++) {
			const TopoDS_Shape& aShape = aReader.Shape(i);
			data->shapes.push_back(aShape);
		}
	}
	catch(...) {
		data->message ="caught C++ exception in readStep";
		data->retValue = 1;
		return;
		
	}

}

void _readStepAsyncAfter(uv_work_t *req,int status) {		

	data_t* data = (data_t *) req->data;

	//xx TryCatch try_catch;

	if (data->retValue == 0 ) { 

		try {		

			std::list<v8::Local<v8::Object> > shapes;

			for (std::list<TopoDS_Shape >::const_iterator it = data->shapes.begin(); it != data->shapes.end() ; it++) {
				const TopoDS_Shape& aShape = (*it);
				extractShape(aShape, shapes);
			}

			Local<Array> arr = convert(shapes); 
			Local<Value> err = Integer::New(0);
			Local<Value> argv[2] = { err, arr };
			Local<Value>  res = data->callback->Call(Context::GetCurrent()->Global(), 2, argv);

		} catch(...) {

			Local<Value> argv[2] = {Integer::New(-3), Local<Value>(String::New(" exception in trying to build shapes"))  };
			Local<Value>  res =  data->callback->Call(Context::GetCurrent()->Global(), 2, argv);

		}
	} else {
		Local<Value> argv[2] = {Integer::New(data->retValue), Local<Value>(String::New(data->message.c_str())) };
		Local<Value>  res =  data->callback->Call(Context::GetCurrent()->Global(), 2, argv);
	}

	uv_close(reinterpret_cast<uv_handle_t*>(&data->async),0);

	data->callback.Dispose();
	if (!data->progressCallback.IsEmpty()) {
		data->progressCallback.Dispose();
	}
	delete data;
}

int uv_queue_work__(uv_loop_t* loop, uv_work_t* req,    uv_work_cb work_cb, uv_after_work_cb after_work_cb)
{
	work_cb(req);
	after_work_cb(req,0); 
	return 0;
}
void readStepAsync(const std::string& filename,v8::Local<Function> callback,v8::Local<Function> progressCallback)
{
	data_t* data = new data_t;
	data->req.data = data;
	data->filename = filename;
	data->callback = Persistent<Function>::New(callback);
	if (!progressCallback.IsEmpty()) {
		data->progressCallback = Persistent<Function>::New(progressCallback);
	}
	uv_queue_work(uv_default_loop(), &data->req, _readStepAsync, _readStepAsyncAfter);
}

v8::Handle<Value> readSTEP(const v8::Arguments& args)
{
  
	if (!mutex_initialised) { uv_mutex_init(&stepOperation_mutex);mutex_initialised = true;}

	HandleScope scope;
	std::string filename;
	if (!extractFileName(args[0],filename)) {
		return ThrowException(Exception::TypeError(String::New("expecting a file name")));
	}
	v8::Local<Function> callback;
	if (!extractCallback(args[1],callback)) {
		return ThrowException(Exception::TypeError(String::New("expecting a callback function")));
	}
	v8::Local<Function> progressCallback;
	if (!extractCallback(args[2],progressCallback)) {
		// return ThrowException(Exception::TypeError(String::New("expecting a callback function")));
	}

	Local<Object> global = v8::Context::GetCurrent()->Global();
	readStepAsync(filename,callback,progressCallback);

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


#undef Handle



