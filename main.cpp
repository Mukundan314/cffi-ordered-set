#define NDEBUG

#include <algorithm>
#include <vector>
#include <array>
#include <cstdint>
// #include <cassert>

template<typename T>
struct ordered_set_small {
    using Size = uint8_t;
    static constexpr int max_size = 16;
    Size size;
    std::array<T, max_size> val;
    ordered_set_small() : size(0) {}

    T get(int k) {
        // assert(0 <= k && k < size);
        return val[k];
    }
    
    // ソート順を保ってxを追加
    // 重複する場合何もせずfalseを返す
    bool insert(T x) {
        int i = 0;
        while (i < size && val[i] < x) {
            i++;
        }
        if (i < size && val[i] == x) return false;
        for (int j = size; j > i; j--) {
            val[j] = val[j - 1];
        }
        val[i] = x;
        size++;
        return true;
    }

    // 消せた場合trueを返す
    bool erase(T x) {
        int i = 0;
        while (i < size && val[i] < x) {
            i++;
        }
        if (i == size || val[i] != x) return false;
        for (int j = i; j + 1 < size; j++) {
            val[j] = val[j + 1];
        }
        size--;
        return true;
    }

    ordered_set_small split_half() {
        // assert(size == max_size);
        int r = max_size / 2;
        ordered_set_small res;
        for (int i = r; i < max_size; i++) {
            res.val[i - r] = val[i];
        }
        res.size = max_size - r;
        size = r;
        return res;
    }

    // x未満の数
    int lb(T x) {
        for (int i = 0; i < size; i++) {
            if (val[i] >= x) {
                return i;
            }
        }
        return size;
    }

    // x以下の数
    int ub(T x) {
        for (int i = 0; i < size; i++) {
            if (val[i] > x) {
                return i;
            }
        }
        return size;
    }

    T min() {
        return val[0];
    }

    T max() {
        return val[size - 1];
    }
};


template<typename T>
struct ordered_set {
  private:
    struct node {
        int h, _size;
        ordered_set_small<T> x;
        node *l, *r;
        node() : h(1), _size(0), l(nullptr), r(nullptr) {}
        node(ordered_set_small<T> x) : h(1), _size(x.size), x(x), l(nullptr), r(nullptr) {}
        int balanace_factor() {
            return (l ? l->h : 0) - (r ? r->h : 0);
        }
    };

    static int size(node *v) { return v ? v->_size : 0; }

    static void update(node *v) {
        v->h = std::max(v->l ? v->l->h : 0, v->r ? v->r->h : 0) + 1;
        v->_size = v->x.size;
        if (v->l) {
            v->_size += v->l->_size;
        }
        if (v->r) {
            v->_size += v->r->_size;
        }
    }

    node *rotate_right(node *v) {
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }
    
    node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    node *balance(node *v) {
        int bf = v->balanace_factor();
        // assert(-2 <= bf && bf <= 2);
        if (bf == 2) {
            if (v->l->balanace_factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }
            return rotate_right(v);
        } else if(bf == -2) {
            if (v->r->balanace_factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }
            return rotate_left(v);
        }
        return v;
    }

    node *insert_leftmost(node *v, node *u) {
        if (!v) return u;
        v->l = insert_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

    node *cut_leftmost(node *v, node* &u) {
        if (!v->l) {
            u = v;
            return v->r;
        }
        v->l = cut_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

  public:
    node *root;
    ordered_set() : root(nullptr) {}
    // ソート済かつユニーク
    ordered_set(const int N, const int *val) : root(nullptr) {
        if (N == 0) return;
        std::vector<node*> nodes;
        int B = 12;
        int M = (N + B - 1) / B;
        for (int i = 0; i < M; i++) {
            int l = B * i, r = std::min(N, l + B);
            ordered_set_small<T> x;
            for (int j = l; j < r; j++) {
                // assert(!j || val[j - 1] < val[j]);
                x.val[j - l] = val[j];
            }
            x.size = r - l;
            nodes.push_back(new node(x));
        }
        auto dfs = [&](auto &&dfs, int l, int r) -> node* {
            int m = (l + r) / 2;
            if (l < m) nodes[m]->l = dfs(dfs, l, m);
            if (m + 1 < r) nodes[m]->r = dfs(dfs, m + 1, r);
            update(nodes[m]);
            return nodes[m];
        };
        root = dfs(dfs, 0, nodes.size());
    }

    int size() {
        return size(root);
    }

    // 追加できたか
    bool insert(T x) {
        bool res = false;
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v) {
                v = new node();
                v->x.val[0] = x;
                v->x.size = 1;
                res = true;
                update(v);
                return v;
            }
            if (v->l && x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (v->r && x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                res = v->x.insert(x);
                if (v->x.size == ordered_set_small<T>::max_size) {
                    node *u = new node(v->x.split_half());
                    update(u);
                    v->r = insert_leftmost(v->r, u);   
                }
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
        return res;
    }

    // 削除できたか
    bool erase(T x) {
        bool res = false;
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v) return nullptr;
            if (x < v->x.min()) {
                v->l = dfs(dfs, v->l);
            } else if (x > v->x.max()) {
                v->r = dfs(dfs, v->r);
            } else {
                res = v->x.erase(x);
                if (v->x.size == 0) {
                    if (!v->r || !v->l) {
                        return (!v->r ? v->l : v->r);
                    } else {
                        node *u = nullptr;
                        node *r = cut_leftmost(v->r, u);
                        u->l = v->l;
                        u->r = r;
                        update(u);
                        return balance(u);
                    }
                }
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
        return res;
    }

    bool find(T x) {
        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                v = v->l;
            } else if(x > v->x.max()) {
                v = v->r;
            } else {
                int idx = v->x.lb(x);
                return idx < v->x.size && v->x.get(idx) == x;
            }
        }
        return false;
    }

    // x未満の最大要素 (ない場合はfalse)
    std::pair<bool, T> lt(T x) {
        T res = x;
        node *v = root;
        while (v) {
            if (x <= v->x.min()) {
                v = v->l;
            } else if(x > v->x.max()) {
                res = v->x.max();
                v = v->r;
            } else {
                int idx = v->x.lb(x);
                // assert(idx);
                return {true, v->x.get(idx - 1)};
            }
        }
        if (res == x) return {false, x};
        return {true, res};
    }

    // x以下の最大要素 (ない場合はfalse)
    std::pair<bool, T> le(T x) {
        return lt(x + 1);
    }

    // xより大きい最小要素 (ない場合はfalse)
    std::pair<bool, T> gt(T x) {
        T res = x;
        node *v = root;
        while (v) {
            if (x < v->x.min()) {
                res = v->x.min();
                v = v->l;
            } else if(x >= v->x.max()) {
                v = v->r;
            } else {
                int idx = v->x.ub(x);
                // assert(idx != v->x.size);
                return {true, v->x.get(idx)};
            }
        }
        if (res == x) return {false, x};
        return {true, res};
    }

    // x以上の最小要素 (ない場合はfalse)
    std::pair<bool, T> ge(T x) {
        return gt(x - 1);
    }

    // k番目に小さい要素 (ない場合はfalse)
    std::pair<bool, T> kth_smallest(int k) {
        if (size() <= k) return {false, T()};
        node *v = root;
        while (true) {
            int lsize = size(v->l);
            if (k < lsize) {
                v = v->l;
            } else if(k < lsize + v->x.size) {
                return {true, v->x.get(k - lsize)};
            } else {
                k -= lsize + v->x.size;
                v = v->r;
            }
        }
    }
    
    // r未満の要素の数
    int rank(T r) {
        node *v = root;
        int res = 0;
        while (v) {
            if (r <= v->x.min()) {
                v = v->l;
            } else if(r <= v->x.max() + 1) {
                res += size(v->l);
                return res + v->x.lb(r);
            } else {
                res += size(v->l) + v->x.size;
                v = v->r;
            }
        }
        return res;
    }

    std::vector<T> to_list() {
        if (!root) return {};
        std::vector<T> res;
        auto dfs = [&](auto &&dfs, node *v) -> void {
            if (v->l) dfs(dfs, v->l);
            for (int i = 0; i < v->x.size; i++) {
                res.push_back(v->x.get(i));
            }
            if (v->r) dfs(dfs, v->r);
        };
        dfs(dfs, root);
        return res;
    }
};

extern "C" {

void *T_new(int n, int *data);
void T_insert(void *t_ptr, int x);
void T_erase(void *t_ptr, int x);
int T_kth(void *t_ptr, int k);
int T_rank(void *t_ptr, int x);
int T_le(void *t_ptr, int x);
int T_ge(void *t_ptr, int x);

using T = ordered_set<int>;

void *T_new(int n, int *data) {
  auto t_ptr = new T(n, data);
  return t_ptr;
}

void T_insert(void *t_ptr, int x) {
  auto& t = *static_cast<T*>(t_ptr);
  t.insert(x);
}

void T_erase(void *t_ptr, int x) {
  auto& t = *static_cast<T*>(t_ptr);
  t.erase(x);
}

int T_kth(void *t_ptr, int k) {
  auto& t = *static_cast<T*>(t_ptr);
  auto [f, y] = t.kth_smallest(k);
  return f ? y : -1;
}

int T_rank(void *t_ptr, int x) {
  auto& t = *static_cast<T*>(t_ptr);
  return t.rank(x);
}

int T_le(void *t_ptr, int x) {
  auto& t = *static_cast<T*>(t_ptr);
  auto [f, y] = t.le(x);
  return f ? y : -1;
}

int T_ge(void *t_ptr, int x) {
  auto& t = *static_cast<T*>(t_ptr);
  auto [f, y] = t.ge(x);
  return f ? y : -1;
}

}
