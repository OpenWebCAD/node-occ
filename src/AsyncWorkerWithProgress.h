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
  uv_async_t async;
protected:
  std::string _filename;
public:
  ProgressData m_data;

public:
  AsyncWorkerWithProgress(
    Nan::Callback *callback,
    Nan::Callback* progressCallback,
    std::string*  pfilename
  )
    : Nan::AsyncWorker(callback) , _progressCallback(progressCallback)
  {
    _filename = *pfilename; 
    delete pfilename;
    async.data = this;
    uv_async_init(Nan::GetCurrentEventLoop(), &async, AsyncProgress_);

  }

 inline static void AsyncClose_(uv_handle_t* handle) {
    AsyncWorkerWithProgress* worker = static_cast<AsyncWorkerWithProgress*>(handle->data);
    delete worker;
    handle->data = nullptr;
  }

  virtual void Destroy() {
    uv_close(reinterpret_cast<uv_handle_t*>(&async), AsyncClose_);
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
    uv_async_send(&this->async);
  };


  static NAUV_WORK_CB(AsyncProgress_) {
    AsyncWorkerWithProgress *worker = static_cast<AsyncWorkerWithProgress*>(async->data);
    worker->notify_progress1();
  }

  /**
  * 
  * Note:
  *    - this method is called in the main loop thread.
  *    - there is no garanty that this method will be called for each send_notify_progress
  */
  void notify_progress1() {
  
    Nan::HandleScope scope;
    if (_progressCallback && !_progressCallback->IsEmpty()) {
      v8::Local<v8::Value> argv[2] = {
        Nan::New<v8::Number>(this->m_data.m_progress),
        Nan::New<v8::Integer>((int)this->m_data.m_percent)
      };
      _progressCallback->Call(2,argv, async_resource);
    }
  }
};