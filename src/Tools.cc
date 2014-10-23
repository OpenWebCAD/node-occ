#include "Tools.h"

#include "IFSelect_ReturnStatus.hxx"

#include "Shape.h"
#include "Solid.h"

#include <list>
#include <strstream>
#include "AsyncWorkerWithProgress.h"

//
// ref : http://nikhilm.github.io/uvbook/threads.html
//
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
    if (NanHasInstance(Solid::_template,obj)) {
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


NAN_METHOD(writeSTEP)
{
  NanScope();

  std::string filename;
  if (!extractFileName(args[0],filename)) {
    NanThrowError("expecting a file name");
    NanReturnUndefined();
  }

  std::list<Shape*>  shapes;
  for (int i=1; i<args.Length(); i++) {
    extractShapes(args[i],shapes);
  }

  if (shapes.size()==0) {
    NanReturnValue(NanNew<Boolean>(false));
  }

  try {
    STEPControl_Writer writer;
    IFSelect_ReturnStatus status;

    //xx Interface_Static::SetCVal("xstep.cascade.unit","M");
    //xx Interface_Static::SetIVal("read.step.nonmanifold", 1);

    for (std::list<Shape*>::iterator it = shapes.begin();it != shapes.end();it++) {
      status = writer.Transfer((*it)->shape(), STEPControl_AsIs);
      if (status != IFSelect_RetDone) {
        return NanThrowError("Failed to write STEP file");
      }
    }
    status = writer.Write(filename.c_str());
  } CATCH_AND_RETHROW("Failed to write STEP file ");


  NanReturnValue(NanNew<Boolean>(true));
}

NAN_METHOD(writeBREP)
{
  NanScope();
  std::string filename;
  if (!extractFileName(args[0],filename)) {
    NanThrowError("expecting a file name");
    NanReturnUndefined();
  }
  std::list<Shape*>  shapes;
  for (int i=1; i<args.Length(); i++) {
    extractShapes(args[i],shapes);
  }
  if (shapes.size()==0) {
    NanReturnValue(NanNew<Boolean>(false));
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
  NanReturnValue(NanNew<Boolean>(true));
}

NAN_METHOD(writeSTL)
{
  NanScope();
  std::string filename;
  if (!extractFileName(args[0],filename)) {
    return NanThrowError("expecting a file name");
  }
  std::list<Shape*>  shapes;
  for (int i=1; i<args.Length(); i++) {
    extractShapes(args[i],shapes);
  }
  if (shapes.size()==0) {
    NanReturnValue(NanNew<Boolean>(false));
  }
  try {
    BRep_Builder B;
    TopoDS_Compound C;
    B.MakeCompound(C);
    for (std::list<Shape*>::iterator it = shapes.begin();it != shapes.end();it++) {
      TopoDS_Shape shape = (*it)->shape();
      B.Add(C,shape);
    }
    StlAPI_Writer writer;
    writer.ASCIIMode() = Standard_False;
    writer.Write(C,filename.c_str(),Standard_True);

  } CATCH_AND_RETHROW("Failed to write STL file ");
  NanReturnValue(NanNew<Boolean>(true));
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
  Local<Array> arr = NanNew<Array>((int)shapes.size());
  int i=0;
  for (std::list<Local<Object> >::iterator it = shapes.begin();it != shapes.end();it++) {
    arr->Set(i,*it); 
    i++;	
  }
  return arr;
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




class MyProgressIndicator : public Message_ProgressIndicator
{  
  ProgressData* m_data;
  AsyncWorkerWithProgress* _worker;
public:
  MyProgressIndicator(AsyncWorkerWithProgress* worker);

  void notify_progress();
  virtual Standard_Boolean Show(const Standard_Boolean force);
};



MyProgressIndicator::MyProgressIndicator(AsyncWorkerWithProgress* worker)
  :Message_ProgressIndicator(),_worker(worker)
{
  m_data = &_worker->m_data;
}


Standard_Boolean MyProgressIndicator::Show(const Standard_Boolean force)
{

  double value = this->GetPosition();
  double delta = (value-this->m_data->m_lastValue);

  if ( delta > 0.01) {
    this->m_data->m_percent  = value;
    this->m_data->m_progress = int(delta*1000);// this->GetPosition();
    this->m_data->m_lastValue = value;
    //_worker->send_notify_progress();

  }
  return Standard_False;
}





//http://free-cad.sourceforge.net/SrcDocu/df/d7b/ImportStep_8cpp_source.html


class StepAsyncReadWorker : public AsyncWorkerWithProgress {
public:
  StepAsyncReadWorker(NanCallback *callback,NanCallback* progressCallback,std::string* pfilename)
  : AsyncWorkerWithProgress(callback,progressCallback,pfilename)
  {   
  }
  ~StepAsyncReadWorker() {

  }

  void Execute ();
  void HandleOKCallback();
protected:
  int retValue;
  std::string message;
  std::list<TopoDS_Shape > shapes;
};


void StepAsyncReadWorker::HandleOKCallback() {

  NanScope();

	if (this->retValue == 0 ) { 

		try {		

			std::list<v8::Local<v8::Object> > jsshapes;

			for (std::list<TopoDS_Shape >::iterator it = shapes.begin(); it != shapes.end() ; it++) {
				const TopoDS_Shape& aShape = (*it);
				extractShape(aShape, jsshapes);
			}

			Local<Array> arr = convert(jsshapes); 
			Local<Value> err = NanNew<Integer>(0);
			Local<Value> argv[2] = { err, arr };
			callback->Call(2, argv);

		} catch(...) {

			Local<Value> argv[2] = {NanNew<Integer>(-3), Local<Value>(NanNew(" exception in trying to build shapes"))  };
			callback->Call(2, argv);

		}
	} else {
		Local<Value> argv[2] = {NanNew<Integer>(retValue), Local<Value>(NanNew(message.c_str())) };
		callback->Call(2, argv);
	}	
}

void StepAsyncReadWorker::Execute() {

  MutexLocker _locker(stepOperation_mutex);

  void* data =  request.data;
  retValue = 0;

  occHandle(Message_ProgressIndicator) progress = new MyProgressIndicator(this) ;

  progress->SetScale(1,100,1);
  progress->Show();    

  try {

    STEPControl_Reader aReader;


    Interface_Static::SetCVal("xstep.cascade.unit","mm");
    Interface_Static::SetIVal("read.step.nonmanifold", 1);
    Interface_Static::SetIVal("read.step.product.mode",1);

    progress->NewScope(5,"reading");

    if (aReader.ReadFile(_filename.c_str()) != IFSelect_RetDone) {

      std::strstream str;
      str << " cannot read STEP file " << _filename << std::ends;
      std::cerr << "cannot read "<< std::endl;

      message = str.str();

      // Local<Value> argv[] = { Local<Value>(String::New())  };
      //  Local<Value>  res =  callback->Call(global, 1, argv);
      // NanReturnUndefined();
      progress->EndScope();
      progress->SetValue(105.0);
      progress->Show();    

      retValue = 1;
      return;

    }
    progress->EndScope();
    progress->Show();       


    progress->NewScope(95,"transfert");
    progress->Show();        
    aReader.WS()->MapReader()->SetProgress(progress);


    // Root transfers
    int nbr = aReader.NbRootsForTransfer();

    Standard_Boolean failsonly = Standard_False;
    aReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);

    progress->SetRange(0,nbr);
    int mod = nbr/10+1;
    for (int n = 1; n<= nbr; n++) {

      Standard_Boolean ok = aReader.TransferRoot(n);

      Standard_Integer nbs = aReader.NbShapes();
      if (!ok || nbs == 0) {
        continue; // skip empty root
      }
      if ((n+1)%mod==0) { progress->Increment(); }
    }     

    aReader.WS()->MapReader()->SetProgress(0);
    progress->EndScope();
    progress->Show(); 

    TopoDS_Shape aResShape;
    BRep_Builder B;
    TopoDS_Compound compound;
    B.MakeCompound(compound);



    int nbs = aReader.NbShapes();
    for (int i=1; i<=nbs; i++) {
      const TopoDS_Shape& aShape = aReader.Shape(i);
      B.Add(compound, aShape);

      this->shapes.push_back(aShape);
    }

    aResShape = compound;

    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aResShape, anIndices);

    occHandle(Interface_InterfaceModel) Model = aReader.WS()->Model();
    occHandle(XSControl_TransferReader) TR = aReader.WS()->TransferReader();

    if (!TR.IsNull()) {
      occHandle(Transfer_TransientProcess) TP = TR->TransientProcess();
      occHandle(Standard_Type) tPD     = STANDARD_TYPE(StepBasic_ProductDefinition);
      occHandle(Standard_Type) tNAUO   = STANDARD_TYPE(StepRepr_NextAssemblyUsageOccurrence);
      occHandle(Standard_Type) tShape  = STANDARD_TYPE(StepShape_TopologicalRepresentationItem);
      occHandle(Standard_Type) tGeom   = STANDARD_TYPE(StepGeom_GeometricRepresentationItem);

      Standard_Integer nb = Model->NbEntities();

      cout << " nb entities =" << nb << std::endl;

      for (Standard_Integer ie = 1; ie <= nb; ie++) {

        occHandle(Standard_Transient) enti = Model->Value(ie);

        occHandle(TCollection_HAsciiString) aName;

        if (enti->DynamicType() == tNAUO) {
          occHandle(StepRepr_NextAssemblyUsageOccurrence) NAUO = occHandle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(enti);
          if (NAUO.IsNull()) continue;

          Interface_EntityIterator subs = aReader.WS()->Graph().Sharings(NAUO);
          for (subs.Start(); subs.More(); subs.Next()) {
            occHandle(StepRepr_ProductDefinitionShape) PDS = occHandle(StepRepr_ProductDefinitionShape)::DownCast(subs.Value());
            if (PDS.IsNull()) continue;
            occHandle(StepBasic_ProductDefinitionRelationship) PDR = PDS->Definition().ProductDefinitionRelationship();
            if (PDR.IsNull()) continue;
            if (PDR->HasDescription() &&  PDR->Description()->Length() >0 ) {
              aName = PDR->Description();
            } else if (PDR->Name()->Length() >0) {
              aName = PDR->Name();
            } else {
              aName = PDR->Id();
            }
          }
          // find proper label
          TCollection_ExtendedString str (aName->String() );
        } else  if ( enti->IsKind( tShape ) || enti->IsKind(tGeom)) {
          aName = occHandle(StepRepr_RepresentationItem)::DownCast(enti)->Name();
        } else if (enti->DynamicType() == tPD)    {
          occHandle(StepBasic_ProductDefinition) PD = occHandle(StepBasic_ProductDefinition)::DownCast(enti);
          if (PD.IsNull()) continue;
          occHandle(StepBasic_Product) Prod = PD->Formation()->OfProduct();
          aName = Prod->Name();
        }else {
          continue;
        }
        if ( aName->UsefullLength() < 1 )
          continue;
        // skip 'N0NE' name
        if ( aName->UsefullLength() == 4 &&toupper (aName->Value(1)) == 'N' &&toupper (aName->Value(2)) == 'O' && toupper (aName->Value(3)) == 'N' && toupper (aName->Value(4)) == 'E')   
          continue; 
        /*             
        // special check to pass names like "Open CASCADE STEP translator 6.3 1"
        TCollection_AsciiString aSkipName ("Open CASCADE STEP translator");
        if (aName->Length() >= aSkipName.Length()) {
        if (aName->String().SubString(1, aSkipName.Length()).IsEqual(aSkipName))
        continue;
        }

        */
        TCollection_ExtendedString aNameExt (aName->ToCString());

        cout << " name of part =" << aName->ToCString() << std::endl;
        // find target shape
        occHandle(Transfer_Binder) binder = TP->Find(enti);
        if (binder.IsNull()) continue;

        TopoDS_Shape S = TransferBRep::ShapeResult(binder);
        if (S.IsNull()) continue;

        cout << " name of part = ---------" << std::endl;
        // as PRODUCT can be included in the main shape
        // several times, we look here for all iclusions.
        Standard_Integer isub, nbSubs = anIndices.Extent();
        for (isub = 1; isub <= nbSubs; isub++) {
          TopoDS_Shape aSub = anIndices.FindKey(isub);
          if (aSub.IsPartner(S)) {

            cout << " name of part =" << aName->ToCString() << "  shape " << HashCode(aSub,-1) << " " << aSub.ShapeType() << endl;
#if 0
            // create label and set shape
            if (L.IsNull()){
              TDF_TagSource aTag;
              L = aTag.NewChild(theShapeLabel);
              TNaming_Builder tnBuild (L);
              //tnBuild.Generated(S);
              tnBuild.Generated(aSub);
            }
            // set a name
            TDataStd_Name::Set(L, aNameExt);
#endif
          }
        }
        //           }

      }
      // END: Store names
    }
    //
    //
    //
  }
  catch(...) {
    std::cerr << " EXCEPTION in READ STEP" << std::endl;
    message ="caught C++ exception in readStep";
    retValue = 1;
    return;

  }

}

//void _readStepAsyncAfter(uv_work_t *req,int status) {		
//
//  //xx TryCatch try_catch;
//
//  if (data->retValue == 0 ) { 
//
//    try {		
//
//      std::list<v8::Local<v8::Object> > shapes;
//
//      for (std::list<TopoDS_Shape >::const_iterator it = data->shapes.begin(); it != data->shapes.end() ; it++) {
//        const TopoDS_Shape& aShape = (*it);
//        extractShape(aShape, shapes);
//      }
//
//      Local<Array> arr = convert(shapes); 
//      Local<Value> err = NanNew<Integer>(0);
//      Local<Value> argv[2] = { err, arr };
//      Local<Value>  res = NanCallback(data->callback).Call(2, argv);
//
//    } catch(...) {
//
//      Local<Value> argv[2] = {NanNew<Integer>(-3), Local<Value>(NanNew(" exception in trying to build shapes"))  };
//      Local<Value>  res =  NanCallback(data->callback).Call(2, argv);
//
//    }
//  } else {
//    Local<Value> argv[2] = {Integer::New(data->retValue), Local<Value>(NanNew<String>(data->message.c_str())) };
//    Local<Value>  res =  NanCallback(data->callback).Call(2, argv);
//  }
//  dispose(data);
//}




//void readStepAsyncOld(const std::string& filename,v8::Local<Function> callback,v8::Local<Function> progressCallback)
//{
//	data_t* data = new data_t;
//	data->req.data = data;
//	data->filename = filename;
//	data->callback = NanNew<Function>(callback);
//	if (!progressCallback.IsEmpty()) {
//		data->progressCallback = NanNew<Function>(progressCallback);
//	}
//	uv_queue_work(uv_default_loop(), &data->req, _readStepAsync, _readStepAsyncAfter);
//}

void readStepAsync(const std::string& filename,v8::Local<Function> _callback,v8::Local<Function> _progressCallback)
{
  NanCallback* callback = new NanCallback(_callback);
  NanCallback* progressCallback = _progressCallback.IsEmpty() ? NULL :new NanCallback(_progressCallback);
  std::string* pfilename = new std::string(filename);
  NanAsyncQueueWorker(new StepAsyncReadWorker(callback,progressCallback,pfilename)); 
}

NAN_METHOD(readSTEP)
{


  if (!mutex_initialised) { uv_mutex_init(&stepOperation_mutex);mutex_initialised = true;}

  NanScope();

  std::string filename;
  if (!extractFileName(args[0],filename)) {
    NanThrowError("expecting a file name");
    NanReturnUndefined();
  }
  v8::Local<Function> callback;
  if (!extractCallback(args[1],callback)) {
    NanThrowError("expecting a callback function");
    NanReturnUndefined();
  }
  v8::Local<Function> progressCallback;
  if (!extractCallback(args[2],progressCallback)) {
    // OPTIONAL !!!
    // NanThrowError("expecting a callback function");
  }

  readStepAsync(filename,callback,progressCallback);
  NanReturnUndefined();
}



class BRepAsyncReadWorker : public StepAsyncReadWorker {
public:
  BRepAsyncReadWorker(NanCallback *callback,NanCallback* progressCallback,std::string* pfilename)
  : StepAsyncReadWorker(callback,progressCallback,pfilename)
  {   
  }
  ~BRepAsyncReadWorker() {

  }

  void Execute ();

};


void BRepAsyncReadWorker::Execute()
{

  this->retValue = 0;
  std::string filename = this->_filename;

  try {
    occHandle(Message_ProgressIndicator) progress = new MyProgressIndicator(this) ;
    progress->SetScale(1,100,1);
    progress->Show();    

    // read brep-file
    TopoDS_Shape shape;
    BRep_Builder aBuilder;
    if (!BRepTools::Read(shape, filename.c_str(), aBuilder,progress)) {
      std::strstream str;
      str << " cannot read BREP file " << filename << std::ends;
      std::cerr << str.str() << std::endl;
      this->message = str.str();
      this->retValue = 1;
      progress->SetValue(100.0);
      progress->Show();
      return;
    }
    this->shapes.push_back(shape);
    progress->SetValue(100.0);
    progress->Show();
  } 
  catch(...) {
    this->message ="caught C++ exception in _readBREPAsync";
    this->retValue = -3;
    return;
  }
}

void readBREPAsync(const std::string& filename,v8::Local<Function> _callback,v8::Local<Function> _progressCallback)
{
  NanCallback* callback = new NanCallback(_callback);
  NanCallback* progressCallback = _progressCallback.IsEmpty() ? NULL : new NanCallback(_progressCallback);
  std::string* pfilename = new std::string(filename);
  NanAsyncQueueWorker(new BRepAsyncReadWorker(callback,progressCallback,pfilename)); 
}


NAN_METHOD(readBREP)
{

  NanScope();

  std::string filename;
  if (!extractFileName(args[0],filename)) {
    NanThrowError("expecting a file name");
    NanReturnUndefined();

  }
  v8::Local<Function> callback;
  if (!extractCallback(args[1],callback)) {
    NanThrowError("expecting a callback function");
    NanReturnUndefined();

  }
  v8::Local<Function> progressCallback;
  if (!extractCallback(args[2],progressCallback)) {
    // return NanThrowError("expecting a callback function");
  }
  readBREPAsync(filename,callback,progressCallback);
  NanReturnUndefined();
}
#undef Handle



