#include "nan.h"

struct ProgressData {
public:
  ProgressData();
  double m_lastValue;
  double m_percent;
  double m_progress;
};

inline ProgressData::ProgressData()
:m_lastValue(0)
,m_percent(0)
,m_progress(0)
{}

class AsyncWorkerWithProgress : public Nan::AsyncWorker {

  Nan::Callback* _progressCallback;
  uv_async_t* async;
protected:
  std::string _filename;
public:
  ProgressData m_data;

public:
  AsyncWorkerWithProgress(Nan::Callback *callback,Nan::Callback* progressCallback,std::string*  pfilename)
    : Nan::AsyncWorker(callback) , _progressCallback(progressCallback)
  {
    async = new uv_async_t();
    _filename = *pfilename; 
    delete pfilename;

    uv_async_init(uv_default_loop(),async,AsyncWorkerWithProgress::notify_progress);
    async->data = this;

  }
 inline static void AsyncClose_(uv_handle_t* handle) {
    AsyncWorkerWithProgress* worker = static_cast<AsyncWorkerWithProgress*>(handle->data);
    delete reinterpret_cast<uv_async_t*>(handle);
    delete worker;
  }
  virtual void Destroy() {
    uv_close(reinterpret_cast<uv_handle_t*>(async), AsyncClose_);
  }
  ~AsyncWorkerWithProgress() {

    if (_progressCallback) {
      delete _progressCallback;
    }

  }




  /**
  * Method wich is called in the context of a working thread
  */
  void Execute () =0;

  /**
  * send a message to the main loop so that a progress callback can be made
  * in the thread of the main loop.
  * this is equivalent of asking uv to call notify_progress in the context of the
  * main loop in the near future.
  */
  void send_notify_progress() {
    uv_async_send(this->async);
  };

#if NODE_MODULE_VERSION >= 14 // 12
static void notify_progress(uv_async_t* handle) 
#else
static void notify_progress(uv_async_t* handle,int status/*unused*/) 
#endif
  {
    AsyncWorkerWithProgress *worker = static_cast<AsyncWorkerWithProgress*>(handle->data);
    worker->notify_progress();
  }

  /**
  * 
  * Note:
  *    - this method is called in the main loop thread.
  *    - there is no garanty that this method will be called for each send_notify_progress
  */
  void notify_progress() {

    Nan::HandleScope scope;
    //xx printf("notify_progress %lf %d\n",m_data.percent,m_data.progress);
    if (_progressCallback && !_progressCallback->IsEmpty()) {
      //xx printf("notify_progress %lf %d\n",m_data.percent,m_data.progress);
      v8::Local<v8::Value> argv[2] = {
        Nan::New<v8::Number>(this->m_data.m_progress),
        Nan::New<v8::Integer>((int)this->m_data.m_percent)
      };
      _progressCallback->Call(2,argv);
    }
  }
};