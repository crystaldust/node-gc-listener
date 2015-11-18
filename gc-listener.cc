#include "nan.h"
#include <uv.h>

using Nan::FunctionCallbackInfo;
using Nan::Persistent;

using v8::Value;
using v8::Object;
using v8::Function;
using v8::FunctionTemplate;
using v8::Local;

using v8::GCType;
using v8::GCCallbackFlags;
using v8::V8;
using v8::kGCTypeMarkSweepCompact;

Persistent<Function> gCB_BeforeGC;
Persistent<Function> gCB_AfterGC;



void doNothing(uv_work_t *) {}

NAN_GC_CALLBACK(cbAfterGC) {
  Nan::MakeCallback( Nan::GetCurrentContext()->Global(), Nan::New(gCB_AfterGC), 0, {} );
}

void CallBeforeGC(uv_work_t* request) {
  Nan::MakeCallback( Nan::GetCurrentContext()->Global(), Nan::New(gCB_BeforeGC), 0, {} );
}

NAN_GC_CALLBACK( cbBeforeGC ) {
  uv_queue_work( uv_default_loop(), new uv_work_t(), doNothing, uv_after_work_cb(CallBeforeGC) );
}

NAN_METHOD( SetCB_BeforeGC ) {
  if( info.Length() == 1 && info[0]->IsFunction() ) {
    gCB_BeforeGC.Reset( info[0].As<v8::Function>() );
    Nan::AddGCPrologueCallback( cbBeforeGC, kGCTypeMarkSweepCompact );
  }
}

NAN_METHOD( SetCB_AfterGC ) {
  if( info.Length() == 1 && info[0]->IsFunction() ) {
    gCB_AfterGC.Reset( info[0].As<v8::Function>() );
    Nan::AddGCEpilogueCallback( cbAfterGC, kGCTypeMarkSweepCompact );
  }
}

NAN_MODULE_INIT(Init) {

  Local<Function> beforeGC = Nan::GetFunction( Nan::New<FunctionTemplate>(SetCB_BeforeGC) ).ToLocalChecked();
  Nan::Set(target, Nan::New("before").ToLocalChecked(), beforeGC );

  Local<Function> afterGC  = Nan::GetFunction( Nan::New<FunctionTemplate>(SetCB_AfterGC) ).ToLocalChecked();
  Nan::Set(target, Nan::New("after").ToLocalChecked(), afterGC );
}

NODE_MODULE(gclistener, Init);
