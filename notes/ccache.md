ccache - a fast C/C++ compiler cache. It speeds up recompilation by caching the result of previous compilations and detecting when the same compilation is being done again.

感觉有点像Make Project里在二次编译时只会编译修改过的源文件。但是ccache是在编译器上做的缓存，第一次编译会缓慢一点，但后面即使清除了编译结果(`make clean`)，还是会跳过重复编译过程。

`man ccache`
