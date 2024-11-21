from cffi import FFI

ffibuilder = FFI()

ffibuilder.cdef("""
void *T_new(int n, int *data);
void T_insert(void *t_ptr, int x);
void T_erase(void *t_ptr, int x);
int T_kth(void *t_ptr, int k);
int T_rank(void *t_ptr, int x);
int T_le(void *t_ptr, int x);
int T_ge(void *t_ptr, int x);
""")

ffibuilder.set_source("_output", """
void *T_new(int n, int *data);
void T_insert(void *t_ptr, int x);
void T_erase(void *t_ptr, int x);
int T_kth(void *t_ptr, int k);
int T_rank(void *t_ptr, int x);
int T_le(void *t_ptr, int x);
int T_ge(void *t_ptr, int x);
""", sources=["main.cpp"])

if __name__ == "__main__":
    ffibuilder.compile(verbose=True)
