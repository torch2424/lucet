#include "../quickjs-wasi/quickjs-libc.h"
#include <sightglass.h>

// Set up runtime and context
JSRuntime *rt;
JSContext *ctx;


void hello_world_setup(void *global_ctx, void **ctx_p) {
    (void) global_ctx;
    (void) ctx_p;

    rt = JS_NewRuntime();
    ctx = JS_NewContextRaw(rt);
    JS_AddIntrinsicBaseObjects(ctx);
    JS_AddIntrinsicDate(ctx);
    JS_AddIntrinsicStringNormalize(ctx);
    JS_AddIntrinsicRegExp(ctx);
    JS_AddIntrinsicJSON(ctx);
    JS_AddIntrinsicMapSet(ctx);
    JS_AddIntrinsicTypedArrays(ctx);
    JS_AddIntrinsicPromise(ctx);
}

int hello_world_body(void *ctx) {
    (void) ctx;
    char hello_world[] = "console.log('Hello World');";
    JS_Eval(ctx, hello_world, sizeof(hello_world), "/dev/stdout", JS_EVAL_TYPE_GLOBAL);
}

void hello_world_teardown(void *ctx)
{
    (void) ctx;
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

/*
int main(int argc, char **argv) {
  rt = JS_NewRuntime();
  ctx = JS_NewContextRaw(rt);
  JS_AddIntrinsicBaseObjects(ctx);
  JS_AddIntrinsicDate(ctx);
  JS_AddIntrinsicStringNormalize(ctx);
  JS_AddIntrinsicRegExp(ctx);
  JS_AddIntrinsicJSON(ctx);
  JS_AddIntrinsicMapSet(ctx);
  JS_AddIntrinsicTypedArrays(ctx);
  JS_AddIntrinsicPromise(ctx);

  js_std_add_helpers(ctx, argc, argv);
  js_std_eval_binary(ctx, qjsc_test, qjsc_test_size, 0);
  js_std_loop(ctx);

  JS_FreeContext(ctx);
  JS_FreeRuntime(rt);
  return 0;
}
*/

extern TestsConfig tests_config;

TestsConfig tests_config = { .global_setup    = NULL,
    .global_teardown = NULL,
    .version         = TEST_ABI_VERSION };
