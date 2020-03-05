So building this is difficult (for me at least).

1. Clone down this repo, amke sure you have the [quickjs-wasi](https://github.com/dip-proto/quickjs-wasi) repo submodule.
2. Then, we need the WASM symbol files like (`libquickjs.lto.a`). Install all required dependencies by following [the compiling on linux guide](https://github.com/bytecodealliance/lucet/blob/master/docs/src/Compiling-on-Linux.md#installation-on-any-recent-linux-system-using-the-base-compiler)
3. Run `make` inside of `quickjs-wasi`! And you need to make sure that clang is being used. So you may have to comment out the if block containing: `# use clang instead of gcc`. Also, you may have to change the directories of where binaries are pointing. You must use clang-9 and LLVM 9. And the Makefile was written for macs. So Be sure your clang / LLVM version is 9. And when you see errors on llvm not being found, try replacing the binary paths in the makefile.
4. After this compiles, take the symbol files: `[libquickjs.lto.a, libquickjs.a]` and move them somewhere into this directory.
5. Now we need to compile for the native symbol files `[libquickjs.lto.a, libquickjs.a]`. Comment out all of the `--target=wasm32-wasi` and WASI sysroot flags you can find.  Also, add `-fPIC` to the `CFLAGS` variable. And run `make libquickjs.a` to build the archive files.
6. Move these symbols files into somewhere in this directory.
7. Fill out the `WASM_LIBQUICKJS_ARCHIVE` and `NATIVE_LIBQUICKJS_ARCHIVE` in this Makefile.
8. `make` and hopefully it works!
