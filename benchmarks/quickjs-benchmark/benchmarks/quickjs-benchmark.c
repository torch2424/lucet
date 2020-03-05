#include "../quickjs-wasi/quickjs-libc.h"
#include <sightglass.h>


int hello_world_body(void *ctx) {
    printf("\n");
    printf("Hello!\n");

    (void) ctx;

    // Setup
    // https://github.com/everettjf/quickjs-cpp/blob/4df0334050917223b2a9b80dc2cfe399c7eaf8ab/LICENSE
    JSRuntime *js_rt;
    JSContext *js_ctx;
    js_rt = JS_NewRuntime();
    if (!js_rt) {
        fprintf(stderr, "qjs: cannot allocate JS runtime\n");
        return 1;
    }
    js_ctx = JS_NewContext(js_rt);
    if (!js_ctx) {
        fprintf(stderr, "qjs: cannot allocate JS context\n");
        return 1;
    }
    JS_SetModuleLoaderFunc(js_rt, NULL, js_module_loader, NULL);
    js_std_add_helpers(js_ctx, 0, NULL);

    // Run our JS
    char js_code[] = "console.log('Hello From QuickJs!')";
    JSValue response = JS_Eval(js_ctx, js_code, sizeof(js_code), "<input>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(response)) {
        printf("JS Exception!\n");
        return 1;
    }

    // Free the runtime
    js_std_free_handlers(js_rt);
    JS_FreeContext(js_ctx);
    JS_FreeRuntime(js_rt);
    printf("\n");
}


extern TestsConfig tests_config;

TestsConfig tests_config = { .global_setup    = NULL,
    .global_teardown = NULL,
    .version         = TEST_ABI_VERSION };
