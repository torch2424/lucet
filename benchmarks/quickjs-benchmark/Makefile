LUCET_ROOT:=$(abspath ../..)

# Compiling for Native
CROSS_PREFIX=/usr/lib/llvm-9/bin/
CC=$(CROSS_PREFIX)clang

# Compiling for Wasm
WASI_SDK?=/opt/wasi-sdk
WASI_CC?=$(WASI_SDK)/bin/clang
BINARYEN_DIR?=/opt/binaryen
WASM_OPT?=$(shell command -v wasm-opt || echo $(BINARYEN_DIR)/bin/wasm-opt)

LUCETC:=$(LUCET_ROOT)/target/release/lucetc

LUCET_RUNTIME:=$(LUCET_ROOT)/lucet-runtime
LUCET_WASI:=$(LUCET_ROOT)/lucet-wasi
SIGHTGLASS:=$(LUCET_ROOT)/sightglass

LIBBUILTINS:=$(LUCET_ROOT)/lucet-builtins/build/libbuiltins.so

QUICKJS_BENCHMARKS:=$(LUCET_ROOT)/benchmarks/quickjs-benchmark/benchmarks
QUICKJS_BENCHMARKS_SRCS:=$(shell ls $(QUICKJS_BENCHMARKS)/*.c)
QUICKJS_NATIVE_OBJS:=
QUICKJS_LUCET_OBJS:=
NATIVE_LIBQUICKJS_ARCHIVE:=native-libquickjs-symbols/libquickjs.a
WASM_LIBQUICKJS_ARCHIVE:=wasm-libquickjs-symbols/libquickjs.a

LUCETC_FLAGS:=--opt-level speed_and_size --min-reserved-size 4294967296
COMMON_CFLAGS:=--std=c99 -Ofast -Wall -W -I$(SIGHTGLASS)/include

QUICKJS_NATIVE_CFLAGS:=-march=native -fPIC \
	-DIMPL_REFERENCE -DUSE_LEND \
	-Dmalloc=lend_malloc -Dcalloc=lend_calloc -Dfree=lend_free

BINARYEN_VERSION=86

ifdef CI
	SIGHTGLASS_ARGS:=--quick
endif

default: run

.PHONY: bench
bench: run

.PHONY: run
run: build
	$(LUCET_ROOT)/target/release/sightglass -c sightglass.toml $(SIGHTGLASS_ARGS)

.PHONY: build
build: $(LUCETC)
build: build/native/implementation.so
build: build/lucet/implementation.so
build: build/lucet/module.so
build: $(LUCET_ROOT)/target/release/sightglass

build/native/quickjs/%.o: $(QUICKJS_BENCHMARKS)/%.c
	@mkdir -p $(@D)
	$(CC) $(COMMON_CFLAGS) -fPIC -c $^ -o $@

build/native/implementation.so: $(NATIVE_LIBQUICKJS_ARCHIVE)
build/native/implementation.so: $(patsubst %.c, %.o, $(addprefix build/native/quickjs/, $(notdir $(QUICKJS_BENCHMARKS_SRCS))))
	@mkdir -p $(@D)
	$(CC) -shared -o $@ $^

build/lucet/quickjs/%.o: $(QUICKJS_BENCHMARKS)/%.c
	@mkdir -p $(@D)
	$(WASI_CC) $(COMMON_CFLAGS) -c $^ -o $@

build/lucet/module.wasm.unoptimized: $(patsubst %.c, %.o, $(addprefix build/lucet/quickjs/, $(notdir $(QUICKJS_BENCHMARKS_SRCS))))
	@mkdir -p $(@D)
	$(WASI_CC) $^ -o $@ $(WASM_LIBQUICKJS_ARCHIVE) -nostartfiles -Wl,--no-entry -Wl,--export-all

build/lucet/module.wasm: build/lucet/module.wasm.unoptimized
	$(WASM_OPT) -mvp --disable-mutable-globals -O4 -o $@ $^

# Don't emit the shared object, we need a separate link step below.
build/lucet/module.o: build/lucet/module.wasm $(LIBBUILTINS)
	@mkdir -p $(@D)
	$(LUCETC) $(LUCETC_FLAGS) --emit=obj \
		--bindings=bindings.json \
		--bindings=$(LUCET_WASI)/bindings.json \
		--builtins=$(LIBBUILTINS) build/lucet/module.wasm -o $@

build/lucet/module.clif: build/lucet/module.wasm $(LIBBUILTINS)
	@mkdir -p $(@D)
	$(LUCETC) $(LUCETC_FLAGS) --emit=clif \
		--bindings=bindings.json \
		--bindings=$(LUCET_WASI)/bindings.json \
		--builtins=$(LIBBUILTINS) build/lucet/module.wasm -o $@

# Because liblucet-runtime-c is dynamically loaded (transitively) by implementation.so, we
# need to link the wasm object to the same libraries.
build/lucet/module.so: build/lucet/module.o
	@mkdir -p $(@D)
	$(CC) -rdynamic -shared \
		-L $(dir $(LIBBUILTINS)) \
		-Wl,-rpath $(dir $(LIBBUILTINS)) \
		build/lucet/module.o -lbuiltins -o $@

build/lucet/%.o: %.c
	@mkdir -p $(@D)
	$(CC) --std=c99 -fPIC -D_GNU_SOURCE -g3 $(COMMON_CFLAGS) \
		-fvisibility=default \
		-I$(LUCET_RUNTIME)/include \
		-I$(LUCET_WASI)/include \
		-DWASM_MODULE=$(abspath build/lucet/module.so) \
		-c $^ -o $@

build/lucet/implementation.so: $(LUCET_ROOT)/target/release/liblucet_runtime.so
build/lucet/implementation.so: $(LUCET_ROOT)/target/release/liblucet_wasi.so
build/lucet/implementation.so: build/lucet/wrapper.o
build/lucet/implementation.so: build/lucet/hostcalls.o
build/lucet/implementation.so: $(LIBBUILTINS)
	@mkdir -p $(@D)
	$(CC) -rdynamic -shared \
		-L $(dir $(LIBBUILTINS)) \
		-Wl,-rpath $(dir $(LIBBUILTINS)) \
		build/lucet/wrapper.o build/lucet/hostcalls.o \
		-L $(LUCET_ROOT)/target/release \
		-Wl,-rpath $(LUCET_ROOT)/target/release \
		-llucet_runtime -llucet_wasi -ldl -lbuiltins -o $@

$(LUCET_ROOT)/target/release/sightglass:
	cargo build --release -p sightglass

$(LIBBUILTINS):
	make -C $(LUCET_ROOT)/lucet-builtins

$(LUCETC):
	cargo build --release -p lucetc

.PHONY: $(LUCET_ROOT)/target/release/liblucet_runtime.so
$(LUCET_ROOT)/target/release/liblucet_runtime.so:
	cargo build --release -p lucet-runtime


.PHONY: $(LUCET_ROOT)/target/release/liblucet_wasi.so
$(LUCET_ROOT)/target/release/liblucet_wasi.so:
	cargo build --release -p lucet-wasi

.PHONY: clean
clean:
	-rm -rf build
