#include "../quickjs-wasi/quickjs-libc.h"
#include <sightglass.h>


int hello_world_body(void *runtime_ctx) {
    printf("\n");
    printf("Hello!\n");

    (void) runtime_ctx;

    // Setup
    //rt = JS_NewRuntime();
    // ctx_ = JS_NewContext(rt);
    // https://github.com/everettjf/quickjs-cpp/blob/4df0334050917223b2a9b80dc2cfe399c7eaf8ab/LICENSE
    JSRuntime *rt;
    JSContext *ctx;
    rt = JS_NewRuntime();
    if (!rt) {
        fprintf(stderr, "qjs: cannot allocate JS runtime\n");
        exit(2);
    }
    ctx = JS_NewContext(rt);
    if (!ctx) {
        fprintf(stderr, "qjs: cannot allocate JS context\n");
        exit(2);
    }
    JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
    js_std_add_helpers(ctx, 0, NULL);
    /* system modules */
    js_init_module_std(ctx, "std");
    js_init_module_os(ctx, "os");

    // Run our JS
    char js[] = "console.log('Hello From QuickJs!')";
    int res;
    JSValue response = JS_Eval(ctx, js, sizeof(js), "<input>", 0);
    if (JS_IsException(response)) {
        printf("JS Exception!\n");
        //return 1;
    }

    // Free the runtime
    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    printf("\n");
}


extern TestsConfig tests_config;

TestsConfig tests_config = { .global_setup    = NULL,
    .global_teardown = NULL,
    .version         = TEST_ABI_VERSION };
