#include "nan.h"

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

NAN_GC_CALLBACK(cbAfterGC) {
  Nan::MakeCallback( Nan::GetCurrentContext()->Global(), Nan::New(gCB_AfterGC), 0, {} );
}

NAN_GC_CALLBACK( cbBeforeGC ) {
  Nan::MakeCallback( Nan::GetCurrentContext()->Global(), Nan::New(gCB_BeforeGC), 0, {} );
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

  /*
  Local<Function> beforeGC = Nan::GetFunction( Nan::New<FunctionTemplate>(SetCB_BeforeGC) ).ToLocalChecked();
  Nan::Set(target, Nan::New("before").ToLocalChecked(), beforeGC );
  */

  Local<Function> afterGC  = Nan::GetFunction( Nan::New<FunctionTemplate>(SetCB_AfterGC) ).ToLocalChecked();
  Nan::Set(target, Nan::New("after").ToLocalChecked(), afterGC );
}

NODE_MODULE(gclistener, Init);
