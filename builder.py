import platform
from cffi import FFI


extra_compile_args = []
if platform.system() != "Windows":
    extra_compile_args.append("-std=c++20")
else:
    extra_compile_args.append("/std:c++20")


ffibuilder = FFI()

ffibuilder.cdef(
    """
void *T_new(int n, int *data);
void T_insert(void *t_ptr, int x);
void T_erase(void *t_ptr, int x);
int T_kth(void *t_ptr, int k);
int T_rank(void *t_ptr, int x);
int T_le(void *t_ptr, int x);
int T_ge(void *t_ptr, int x);
"""
)

ffibuilder.set_source(
    "_output",
    """
void *T_new(int n, int *data);
void T_insert(void *t_ptr, int x);
void T_erase(void *t_ptr, int x);
int T_kth(void *t_ptr, int k);
int T_rank(void *t_ptr, int x);
int T_le(void *t_ptr, int x);
int T_ge(void *t_ptr, int x);
""",
    sources=["main.cpp"],
    source_extension=".cpp",
    extra_compile_args=extra_compile_args,
)

if __name__ == "__main__":
    ffibuilder.compile(verbose=True)
