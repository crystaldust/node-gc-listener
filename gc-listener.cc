#include <node.h>
#include <uv.h>
#include <math.h>

using v8::FunctionCallbackInfo;
using v8::Value;
using v8::Function;
using v8::Local;
using v8::Handle;
using v8::Persistent;
using v8::Object;
using v8::Isolate;
using v8::Boolean;
using v8::String;
using v8::Context;
using v8::Number;
using v8::Integer;
using v8::HandleScope;

using v8::GCType;
using v8::GCCallbackFlags;
using v8::V8;

using v8::kGCTypeMarkSweepCompact;

Persistent<Function> g_cb;
Persistent<Object> g_context;

struct Baton {
  uv_work_t req;
  size_t heapUsage;
  GCType type;
  GCCallbackFlags flags;
};

static void noop_work_func(uv_work_t *) { }

static void AsyncMemwatchAfter(uv_work_t* request) {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);


    Baton * b = (Baton *) request->data;

    // do the math in C++, permanent
    // record the type of GC event that occured


    if (
#if NODE_VERSION_AT_LEAST(0,8,0)
        b->type == kGCTypeMarkSweepCompact
#else
        b->flags == kGCCallbackFlagCompacted
#endif
        ) {

        // if there are any listeners, it's time to emit!
        /*
        if (!g_cb.IsEmpty()) {
            Handle<Value> argv[3];

            // magic argument to indicate to the callback all we want to know is whether there are
            // listeners (here we don't)
            argv[0] = Boolean::New(isolate, true);

            Handle<Value> haveListeners = cb->Call( context, 1, argv);

            if (haveListeners->BooleanValue()) {
                double ut= 0.0;

                // ok, there are listeners, we actually must serialize and emit this stats event
                argv[0] = Boolean::New(isolate, false);
                // the type of event to emit
                argv[1] = String::NewFromUtf8(isolate,"stats");
                argv[2] = stats;
                cb->Call(context, 3, argv);
            }
        }
        */

        if( !g_cb.IsEmpty() ) {
          Local<Object> context = Local<Object>::New( isolate, g_context );
          Local<Function> cb = Local<Function>::New( isolate, g_cb );
          cb->Call( context, 0, {} );
        }
    }

    delete b;
}


void cbAfterGc( GCType type, GCCallbackFlags flags )
{
  Baton * baton = new Baton;             
  baton->type = type;                    
  baton->flags = flags;                  
  baton->req.data = (void *) baton;      

  uv_queue_work(uv_default_loop(), &(baton->req), noop_work_func, (uv_after_work_cb)AsyncMemwatchAfter);
}


// Global handle to the after gc callback

void SetPostCB( const FunctionCallbackInfo<Value>& args )
{
  Isolate *isolate = args.GetIsolate();
  if( args.Length() == 1 && args[0]->IsFunction() ) {
    g_cb.Reset( isolate, Handle<Function>::Cast(args[0]) );
    g_context.Reset( isolate, isolate->GetCallingContext()->Global() );
  }
}

void InvokeCB( const FunctionCallbackInfo<Value>& args )
{
  Isolate *isolate = args.GetIsolate();
  if( !g_cb.IsEmpty() ) {
    Local<Function> cb = Local<Function>::New( isolate, g_cb );
    cb->Call( Null(isolate), 0, {} );
  }
}


void init( Local<Object> exports )
{
  //NODE_SET_METHOD( exports, "beforeGC", BeforeGC );
  //NODE_SET_METHOD( exports, "afterGC",  SetPostCB );
  NODE_SET_METHOD( exports, "setCB",  SetPostCB );
  NODE_SET_METHOD( exports, "invokeCB", InvokeCB );

  V8::AddGCEpilogueCallback( cbAfterGc );
}



NODE_MODULE( whatever, init );
