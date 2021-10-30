#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED
#include <climits>
#include <vector>
#include <string>
#include <cstring>
#if __cplusplus >= 201103L
#include <initializer_list>
#endif // __cplusplus

namespace Achibulup
{
#if __cplusplus >= 201103L
#define ACHIBULUP__lval_fun &
#define ACHIBULUP__constexpr_fun11 constexpr
#else
#define ACHIBULUP__lval_fun
#define ACHIBULUP__constexpr_fun11
#endif // __cplusplus

    using std::size_t;
    template<typename Tp>
    struct identity
    {
        typedef Tp type;
    };

    template<typename char_t>
    struct string_view
    {
        const char_t *ptr;
        size_t len;
        ACHIBULUP__constexpr_fun11 string_view() : ptr(), len() {}
        ACHIBULUP__constexpr_fun11 string_view(const char *p, size_t ln) : ptr(p), len(ln) {}
        template<size_t Nm>
        ACHIBULUP__constexpr_fun11 string_view(const char_t (&str)[Nm]) : ptr(str), len(Nm - 1) {}
        template<class traits, class alloc>
        string_view(const std::basic_string<char_t, traits, alloc> &str) : ptr(str.c_str()), len(str.size()) {}
    };


    template<typename char_t, char_t lbound = 0, char_t ubound = (~static_cast<char_t>(0)) ^ (1 << (sizeof(char_t) * CHAR_BIT - 1))>
    class Trie
    {
        struct Trie_access{};
        typedef size_t id_type;

    public:
        typedef char_t char_type;
        typedef size_t size_type;
        class cursor
        {
            id_type value;

        public:
            cursor() : value(0) {}
            cursor(id_type val, Trie_access) : value(val) {}

            typedef id_type hash_type;
            hash_type hash() const
            {
                return value;
            }

            id_type get(Trie_access) const
            {
                return value;
            }

            friend bool operator == (const cursor &l, const cursor &r)
            {
                return l.value == r.value;
            }
            friend bool operator != (const cursor &l, const cursor &r)
            {
                return l.value != r.value;
            }
        };
        static const char_t value_lower_bound = lbound;
        static const char_t value_upper_bound = ubound;
        static const cursor npos;

    private:
        static id_type get(const cursor &index)
        {
            return index.get(Trie_access());
        }
        static const size_type value_range = static_cast<size_type>(ubound - lbound) + 1;
        static const id_type rootid = 1;
        static const id_type nposid = 0;

        struct node
        {
            struct parent_t{
                id_type index;
                char_t value;
                parent_t (id_type id, char_t val) : index(id), value(val) {}
            }parent;
            const id_type index;

            bool ecnt;
            size_type pcnt;
            id_type child[value_range];

            node(id_type par, id_type idx, char_t val = 0) : parent(par, val), index(idx),
                                                  ecnt(false), pcnt(0), child() {}

            bool is_end() const
            {
                return ecnt;
            }
            size_type as_end_count() const
            {
                return ecnt;
            }
            size_type as_prefix_count() const
            {
                return pcnt;
            }

            const node* go_to(id_type _index) const
            {
                return (_index > index) ? (this + (_index - index)) : (this - (index - _index));
            }

            id_type next(char_t val) const
            {
                id_type res = child[val - lbound];
                if(res == nposid) return nposid;
                const node *pos = go_to(res);
                return (pos->pcnt == 0) ? nposid : res;
            }

            size_type length() const
            {
                size_type res = 0;
                const node *cur = this;
                while(cur->index != rootid){
                  ++res;
                  cur = cur->go_to(cur->parent.index);
                }
                return res;
            }

            std::basic_string<char_t> get_string() const
            {
                std::string res;
                if(index != nposid){
                  size_type dep = length();
                  res.resize(dep);
                  const node *cur = this;
                  while(dep--){
                    res[dep] = cur->parent.value;
                  cur = cur->go_to(cur->parent.index);
                  }
                }
                return res;
            }

        };

        std::vector<node> data;
        std::vector<id_type> reuse;
        size_type ncnt;
        size_type wcnt;

        id_type get_next(id_type parent, char_t val)
        {
            id_type res;
            if(reuse.empty()){
              res = data.size();
              data.push_back(node(parent, res, val));
            }
            else{
              res = reuse.back();
              data[res].parent.index = parent;
              data[res].parent.value = val;
              reuse.pop_back();
            }
            return res;
        }

        void init()
        {
            data.reserve(2);
            data.push_back(node(nposid, nposid));
            data.push_back(node(nposid, rootid));
        }

        static bool comp_tree(const node &l, const node &r)
        {
            if(l.ecnt != r.ecnt)
              return false;
            if(l.pcnt != r.pcnt)
              return false;
            for(size_type i = 0; i < value_range; ++i){
              if(l.child[i] == nposid){
                if(r.child[i] != nposid)
                  return false;
              }
              else{
                if(r.child[i] == nposid)
                  return false;
                if(!comp_tree(*l.go_to(l.child[i]), *r.go_to(r.child[i])))
                  return false;
              }
            }
            return true;
        }

    public:
        Trie() : data(), reuse(), ncnt(0), wcnt(0)
        {
            init();
        }

        cursor root() const
        {
            if(data[rootid].pcnt == 0) return npos;
            return cursor(rootid, Trie_access());
        }

        size_type node_count() const
        {
            return ncnt;
        }

        size_type word_count() const
        {
            return wcnt;
        }

        void reserve(size_type count) ACHIBULUP__lval_fun
        {
            data.reserve(count);
        }

        static bool _equal(const Trie &l, const Trie &r)
        {
            if(&l == &r) return true;
            return comp_tree(l.data[rootid], r.data[rootid]);
        }
        static bool _different(const Trie &l, const Trie &r)
        {
            return !_equal(l, r);
        }

        bool is_end(const cursor &index) const
        {
            return data[get(index)].is_end();
        }
        size_type as_end_count(const cursor &index) const
        {
            return data[get(index)].as_end_count();
        }
        size_type as_prefix_count(const cursor &index) const
        {
            return data[get(index)].as_prefix_count();
        }

        cursor prev(const cursor &index) const
        {
            return cursor(data[get(index)].parent.index, Trie_access());
        }

        cursor next(const cursor &index, char_t val) const
        {
            return cursor(data[get(index)].next(val), Trie_access());
        }

        size_type length(const cursor &index) const
        {
            return data[get(index)].length();
        }

        std::basic_string<char_t> get_string(const cursor &index) const
        {
            return data[get(index)].get_string();
        }

        cursor insert(const char_t *str, size_type len) ACHIBULUP__lval_fun
        {
            id_type cur = rootid;
            for(size_type i = 0; i < len; ++i){
              id_type _next = data[cur].child[str[i] - lbound];
              if(_next == nposid){
                _next = get_next(cur, str[i]);
                data[cur].child[str[i] - lbound] = _next;
              }
              cur = _next;
            }
            id_type res = cur;
            if(!data[cur].ecnt){
              data[cur].ecnt = true;
              ++wcnt;
              while(cur != nposid){
                if(data[cur].pcnt == 0) ++ncnt;
                ++data[cur].pcnt;
                cur = data[cur].parent.index;
              }
            }
            return cursor(res, Trie_access());
        }
        cursor insert(string_view<char_t> strv) ACHIBULUP__lval_fun
        {
            return insert(strv.ptr, strv.len);
        }

#if __cplusplus >= 201103
        Trie(std::initializer_list<string_view<char_t>> init_list) : data(), reuse(), ncnt(0), wcnt(0)
        {
            init();
            for(const string_view<char_t> &elem : init_list)
              insert(elem);
        }
#endif // __cplusplus

        cursor find(const char_t *str, size_type len) const ACHIBULUP__lval_fun
        {
            if(root() == npos) return npos;
            cursor cur = root();
            for(size_type i = 0; i < len; ++i){
              cursor _next = next(cur, str[i]);
              if(_next == npos) return npos;
              cur = _next;
            }
            return cur;
        }
        cursor find(string_view<char_t> strv) const
        {
            return find(strv.ptr, strv.len);
        }

        size_type count(const char_t *str, size_type len) const
        {
            return (find(str, len) != npos) ? 1 : 0;
        }
        size_type count(string_view<char_t> strv) const
        {
            return count(strv.ptr, strv.len);
        }

        bool erase(const cursor &index) ACHIBULUP__lval_fun
        {
            id_type pos = get(index);
            if(data[pos].ecnt == 0) return false;
            data[pos].ecnt = 0;
            --wcnt;
            while(pos != rootid){
              --data[pos].pcnt;
              id_type par = data[pos].parent.index;
              if(data[pos].pcnt == 0){
                --ncnt;
                data[par].child[data[pos].parent.value - lbound] = nposid;
                reuse.push_back(pos);
              }
              pos = par;
            }
            --data[rootid].pcnt;
            if(data[rootid].pcnt == 0)
                --ncnt;
            return true;
        }
        bool erase(const char *str, size_type len) ACHIBULUP__lval_fun
        {
            cursor index = find(str, len);
            return index != npos && erase(index);
        }
        bool erase(string_view<char_t> strv) ACHIBULUP__lval_fun
        {
            return erase(strv.ptr, strv.len);
        }

    private :
        class data_t
        {
        protected:
            const node *base;

        public:
            std::basic_string<char_t> word;

        protected:
            data_t() : base(), word() {}
            data_t(const node &_base) : base(&_base), word(_base.get_string()) {}

        public:
            bool is_end() const
            {
                return base->is_end();
            }

            size_type as_end_count() const
            {
                return base->as_end_count();
            }

            size_type as_prefix_count() const
            {
                return base->as_prefix_count();
            }
        };

    public:
        class const_iterator : private data_t
        {
        public:
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef std::ptrdiff_t difference_type;
            typedef const data_t value_type;
            typedef const data_t &reference;
            typedef const data_t *pointer;

            const_iterator() : data_t() {}
            const_iterator(const node &_base) : data_t(_base) {}

            const_iterator& operator ++ ()
            {
                char_t curv = lbound;
                bool ok = false;

                while(ok == false){
                  while(1){
                    id_type pos = data_t::base->child[curv - lbound];
                    if(pos != nposid){
                      const node *ptr = data_t::base->go_to(pos);
                      if(ptr->pcnt != 0){
                        data_t::base = ptr;
                        data_t::word.push_back(curv);
                        ok = true;
                        break;
                      }
                    }

                    if(curv == ubound) break;
                    ++curv;
                  }

                  if(ok) break;

                  while(data_t::base->index != rootid && data_t::base->parent.value == ubound){
                    data_t::base = data_t::base->go_to(data_t::base->parent.index);
                    data_t::word.pop_back();
                  }
                  if(data_t::base->index == rootid){
                    data_t::base = data_t::base->go_to(nposid);
                    ok = true;
                    break;
                  }
                  curv = data_t::base->parent.value + 1;
                  data_t::base = data_t::base->go_to(data_t::base->parent.index);
                  data_t::word.pop_back();
                }

                return *this;
            }
            const_iterator& operator -- ()
            {
                char_t curv;
                if(data_t::base->index != nposid){
                  curv = data_t::base->parent.value;
                  data_t::base = data_t::base->go_to(data_t::base->parent.index);
                  data_t::word.pop_back();
                }
                else{
                  curv = ubound;
                  data_t::base = data_t::base->go_to(rootid);
                }

                if(curv != lbound){
                  --curv;

                  while(1){
                    id_type pos = data_t::base->child[curv - lbound];
                    if(pos != nposid){
                      const node *ptr = data_t::base->go_to(pos);
                      if(ptr->pcnt != 0){
                        data_t::word.push_back(curv);
                        curv = ubound;
                        data_t::base = ptr;
                        continue;
                      }
                    }

                    if(curv == lbound) break;

                    --curv;
                  }
                }

                return *this;
            }

            reference operator * () const
            {
                return *this;
            }
            pointer operator -> () const
            {
                return this;
            }

            friend bool operator == (const const_iterator &l, const const_iterator &r)
            {
                return l.base == r.base;
            }
            friend bool operator != (const const_iterator &l, const const_iterator &r)
            {
                return l.base != r.base;
            }
        };
        typedef const_iterator iterator;

        const_iterator const_iterator_at(const cursor &index) const
        {
            return const_iterator(data[get(index)]);
        }
        const_iterator iterator_at(const cursor &index) const
        {
            return const_iterator_at(index);
        }
        const_iterator cbegin() const
        {
            if(data[rootid].pcnt == 0) return const_iterator(data[nposid]);
            return const_iterator(data[rootid]);
        }
        const_iterator begin() const
        {
            return cbegin();
        }
        const_iterator cend() const
        {
            return iterator_at(npos);
        }
        const_iterator end() const
        {
            return cend();
        }
    };

    template<typename char_t, char_t lbound, char_t ubound>
    const typename Trie<char_t, lbound, ubound>::cursor Trie<char_t, lbound, ubound>::npos = cursor();


    template<typename char_t, char_t lbound, char_t ubound>
    bool operator == (const Trie<char_t, lbound, ubound> &l, const Trie<char_t, lbound, ubound> &r)
    {
        return Trie<char_t, lbound, ubound>::_equal(l, r);
    }

    template<typename char_t, char_t lbound, char_t ubound>
    bool operator != (const Trie<char_t, lbound, ubound> &l, const Trie<char_t, lbound, ubound> &r)
    {
        return Trie<char_t, lbound, ubound>::_different(l, r);
    }




#if __cplusplus < 201103L
    class nullptr_t
    {
        const void* const fill;

    public:
        nullptr_t() : fill() {}

        template<typename Tp>
        operator Tp* () const
        {
            typedef Tp *ptr;
            return ptr();
        }

        template<typename Tp, class C>
        operator Tp C::* () const
        {
            typedef Tp C::* ptr;
            return ptr();
        }

        operator bool () const
        {
            return false;
        }
    };
#define nullptr nullptr_t()

#endif // __cplusplus





    template<typename char_t, char_t lbound = 0, char_t ubound = (~static_cast<char_t>(0)) ^ (1 << (sizeof(char_t) * CHAR_BIT - 1))>
    class Trie_beta
    {

    public:
        typedef char_t char_type;
        typedef size_t size_type;
        static const char_t value_lower_bound = lbound;
        static const char_t value_upper_bound = ubound;

    private:
        struct Trie_access{};
        static const size_type value_range = static_cast<size_type>(ubound - lbound) + 1;
        struct node
        {
            const struct parent_t{
                node *index;
                char_t value;
                parent_t (node *id, char_t val) : index(id), value(val) {}
            }parent;
            node *to_end;

            bool ecnt;
            size_type pcnt;
            node *child[value_range];

            node() : parent(this, 0), ecnt(false), pcnt(0), child() {}
            node(node *par) : parent(par, 0), to_end(this), ecnt(false), pcnt(0), child() {}
            node(node *par, char_t val) : parent(par, val), to_end(par->to_end), ecnt(false), pcnt(0), child() {}

            bool is_end() const
            {
                return ecnt;
            }
            size_type as_end_count() const
            {
                return ecnt;
            }
            size_type as_prefix_count() const
            {
                return pcnt;
            }

            node* next(char_t val) const
            {
                node *pos = child[val - lbound];
                return (pos == nullptr) ? to_end : pos;
            }

            size_type length() const
            {
                size_type res = 0;
                for(const node *cur = this; cur != cur->parent.index; cur = cur->parent.index)
                  ++res;
                return res;
            }

            std::basic_string<char_t> get_string() const
            {
                std::string res;
                if(this != to_end){
                  size_type dep = length();
                  res.resize(dep);
                  const node *cur = this;
                  while(dep--){
                    res[dep] = cur->parent.value;
                    cur = cur->parent.index;
                  }
                }
                return res;
            }
        };

        node *_root;
        size_type ncnt;
        size_type wcnt;

        node* get_next(node *parent, char_t val)
        {
            ++ncnt;
            return new node(parent, val);
        }

        void init()
        {
            node *the_end = new node(_root);
            _root->to_end = the_end;
        }

        static void copy_tree(node *dest, const node *sour)
        {
            dest->ecnt = sour->ecnt;
            dest->pcnt = sour->pcnt;
            for(size_type i = 0; i < value_range; ++i)
              if(sour->child[i]){
                dest->child[i] = new node(dest, lbound + i);
                copy_tree(dest->child[i], sour->child[i]);
              }
        }
        static void destroy_tree(node *cur)
        {
            for(size_type i = 0; i < value_range; ++i)
              if(cur->child[i]) destroy_tree(cur->child[i]);
            delete cur;
        }
        static void assign_tree(node *dest, const node *sour)
        {
            dest->ecnt = sour->ecnt;
            dest->pcnt = sour->pcnt;
            for(size_type i = 0; i < value_range; ++i){
              if(sour->child[i]){
                if(!dest->child[i])
                  dest->child[i] = new node(dest, lbound + i);
                assign_tree(dest->child[i], sour->child[i]);
              }
              else if(dest->child[i])
                destroy_tree(dest);
            }
        }

        bool comp_tree(const node *l, const node *r)
        {
            if(l->ecnt != r->ecnt)
              return false;
            if(l->pcnt != r->pcnt)
              return false;
            for(size_type i = 0; i < value_range; ++i){
              if(!l->child[i]){
                if(r->child[i])
                  return false;
              }
              else{
                if(!r->child[i])
                  return false;
                if(!comp_tree(l->child[i], r->child[i]))
                  return false;
              }
            }
            return true;
        }

    public:

        class cursor
        {
            node *value;

        public:
            cursor() : value() {}
            cursor(node *val, Trie_access) : value(val) {}

            node* get(Trie_access) const
            {
                return value;
            }

            friend bool operator == (const cursor &l, const cursor &r)
            {
                return l.value == r.value;
            }
            friend bool operator != (const cursor &l, const cursor &r)
            {
                return l.value != r.value;
            }
        };
    private:
        static node* get(const cursor &cs)
        {
            return cs.get(Trie_access());
        }
        static cursor to_cursor(node *nd)
        {
            return cursor(nd, Trie_access());
        }

    public:

        Trie_beta() : _root(new node()), ncnt(0), wcnt(0)
        {
            init();
        }

        Trie_beta(const Trie_beta &cpy) : _root(new node()), ncnt(cpy.ncnt), wcnt(cpy.wcnt)
        {
            init();
            copy_tree(_root, cpy._root);
        }

#if __cplusplus >= 201103L
        Trie_beta(Trie_beta &&mov) : _root(mov._root), ncnt(mov.ncnt), wcnt(mov.wcnt)
        {
            mov._root = new node();
            mov.init();
        }
#endif // __cplusplus

        Trie_beta& operator = (const Trie_beta &cpy) ACHIBULUP__lval_fun
        {
            if(&cpy != this){
              ncnt = cpy.ncnt;
              wcnt = cpy.wcnt;
              assign_tree(_root, cpy._root);
            }
            return *this;
        }

#if __cplusplus >= 201103L
        Trie_beta& operator = (Trie_beta &&mov) &
        {
            if(&mov != this){
              size_type tn = ncnt;
              ncnt = mov.ncnt;
              mov.ncnt = tn;
              size_type tw = wcnt;
              wcnt = mov.wcnt;
              mov.wcnt = tw;
              node *tr = _root;
              _root = mov._root;
              mov._root = tr;
            }
            return *this;
        }
#endif // __cplusplus

        cursor npos() const
        {
            return to_cursor(_root->to_end);
        }
        cursor root() const
        {
            if(_root->pcnt == 0) return npos();
            return to_cursor(_root);
        }

        size_type node_count() const
        {
            return ncnt;
        }

        size_type word_count() const
        {
            return wcnt;
        }

        static bool _equal(const Trie_beta &l, const Trie_beta &r)
        {
            if(&l == &r) return true;
            return comp_tree(l._root, r._root);
        }

        static bool _different(const Trie_beta &l, const Trie_beta &r)
        {
            return !_equal(l, r);
        }

        bool is_end(const cursor &cs) const
        {
            return get(cs)->is_end();
        }
        size_type as_end_count(const cursor &cs) const
        {
            return get(cs)->as_end_count();
        }
        size_type as_prefix_count(const cursor &cs) const
        {
            return get(cs)->as_prefix_count();
        }

        cursor prev(const cursor &cs) const
        {
            return to_cursor(get(cs)->parent.index);
        }

        cursor next(const cursor &cs, char_t val) const
        {
            return to_cursor(get(cs)->next(val));
        }

        size_type length(const cursor &cs) const
        {
            return get(cs)->length();
        }

        std::basic_string<char_t> get_string(const cursor &cs) const
        {
            return get(cs)->get_string();
        }

        cursor insert(const char_t *str, size_type len) ACHIBULUP__lval_fun
        {
            node *cur = _root;
            for(size_type i = 0; i < len; ++i){
              node *_next = cur->child[str[i] - lbound];
              if(_next == nullptr){
                _next = get_next(cur, str[i]);
                cur->child[str[i] - lbound] = _next;
              }
              cur = _next;
            }
            node *res = cur;
            if(!cur->ecnt){
              if(_root->pcnt == 0) ++ncnt;
              cur->ecnt = true;
              ++wcnt;
              while(1){
                ++(cur->pcnt);
                if(cur == _root) break;
                cur = cur->parent.index;
              }
            }
            return to_cursor(res);
        }
        cursor insert(string_view<char_t> strv) ACHIBULUP__lval_fun
        {
            return insert(strv.ptr, strv.len);
        }

#if __cplusplus >= 201103
        Trie_beta(std::initializer_list<string_view<char_t>> init_list) : _root(new node()), ncnt(0), wcnt(0)
        {
            init();
            for(const string_view<char_t> &elem : init_list)
              insert(elem);
        }
#endif // __cplusplus

        cursor find(const char_t *str, size_type len) const
        {
            if(_root->pcnt == 0) return npos();
            cursor cur = root();
            for(size_type i = 0; i < len; ++i){
              cursor _next = next(cur, str[i]);
              if(_next == npos()) return npos();
              cur = _next;
            }
            return cur;
        }
        cursor find(string_view<char_t> strv) const
        {
            return find(strv.ptr, strv.len);
        }

        size_type count(const char_t *str, size_type len) const
        {
            return (find(str, len) != npos()) ? 1 : 0;
        }
        size_type count(string_view<char_t> strv) const
        {
            return count(strv.ptr, strv.len);
        }

        bool erase(const cursor &cs) ACHIBULUP__lval_fun
        {
            node *pos = get(cs);
            if(pos->ecnt == 0) return false;
            pos->ecnt = 0;
            --wcnt;
            while(1){
              --(pos->pcnt);
              if(pos->pcnt == 0){
                --ncnt;
                if(pos != _root){
                  size_type val = pos->parent.value - lbound;
                  pos = pos->parent.index;
                  delete pos->child[val];
                  pos->child[val] = nullptr;
                }
              }
              else{
                if(pos == _root) break;
                pos = pos->parent.index;
              }
            }
            return true;
        }
        bool erase(const char *str, size_type len) ACHIBULUP__lval_fun
        {
            cursor cs = find(str, len);
            return cs != npos() && erase(cs);
        }
        bool erase(string_view<char_t> strv) ACHIBULUP__lval_fun
        {
            return erase(strv.ptr, strv.len);
        }

    private :
        class data_t
        {
        protected:
            const node *base;

        public:
            std::basic_string<char_t> word;

        protected:
            data_t(Trie_access) : base(), word() {}
            data_t(const node *_base, Trie_access) : base(_base), word(_base->get_string()) {}

        public:
            bool is_end() const
            {
                return base->is_end();
            }

            size_type as_end_count() const
            {
                return base->as_end_count();
            }

            size_type as_prefix_count() const
            {
                return base->as_prefix_count();
            }
        };

    public:
        class const_iterator : private data_t
        {
        public:
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef std::ptrdiff_t difference_type;
            typedef const data_t value_type;
            typedef const data_t &reference;
            typedef const data_t *pointer;

            const_iterator() : data_t(Trie_access()) {}
            const_iterator(const node *_base) : data_t(_base, Trie_access()) {}

            const_iterator& operator ++ ()
            {
                char_t curv = lbound;
                bool ok = false;

                while(ok == false){
                  while(1){
                    const node *ptr = data_t::base->child[curv - lbound];
                    if(ptr != nullptr){
                      data_t::base = ptr;
                      data_t::word.push_back(curv);
                      ok = true;
                      break;
                    }

                    if(curv == ubound) break;
                    ++curv;
                  }

                  if(ok) break;

                  while(data_t::base->parent.index != data_t::base && data_t::base->parent.value == ubound){
                    data_t::base = data_t::base->parent.index;
                    data_t::word.pop_back();
                  }
                  if(data_t::base->parent.index == data_t::base){
                    data_t::base = data_t::base->to_end;
                    ok = true;
                    break;
                  }
                  curv = data_t::base->parent.value + 1;
                  data_t::base = data_t::base->parent.index;
                  data_t::word.pop_back();
                }

                return *this;
            }
            const_iterator& operator -- ()
            {
                char_t curv;
                if(data_t::base != data_t::base->to_end){
                  curv = data_t::base->parent.value;
                  data_t::word.pop_back();
                }
                else curv = ubound;
                data_t::base = data_t::base->parent.index;

                if(curv != lbound){
                  --curv;

                  while(1){
                    const node *ptr = data_t::base->child[curv - lbound];
                    if(ptr != nullptr){
                      data_t::word.push_back(curv);
                      curv = ubound;
                      data_t::base = ptr;
                      continue;
                    }

                    if(curv == lbound) break;

                    --curv;
                  }
                }

                return *this;
            }

            reference operator * () const
            {
                return *this;
            }
            pointer operator -> () const
            {
                return this;
            }

            friend bool operator == (const const_iterator &l, const const_iterator &r)
            {
                return l.base == r.base;
            }
            friend bool operator != (const const_iterator &l, const const_iterator &r)
            {
                return l.base != r.base;
            }
        };
        typedef const_iterator iterator;

        const_iterator const_iterator_at(const cursor &cs) const
        {
            return const_iterator(get(cs));
        }
        const_iterator iterator_at(const cursor &index) const
        {
            return const_iterator_at(index);
        }
        const_iterator cbegin() const
        {
            if(_root->pcnt == 0) return const_iterator(_root->to_end);
            return const_iterator(_root);
        }
        const_iterator begin() const
        {
            return cbegin();
        }
        const_iterator cend() const
        {
            return iterator_at(npos());
        }
        const_iterator end() const
        {
            return cend();
        }

        ~Trie_beta()
        {
            delete _root->to_end;
            destroy_tree(_root);
        }
    };



    template<typename char_t, char_t lbound, char_t ubound>
    bool operator == (const Trie_beta<char_t, lbound, ubound> &l, const Trie_beta<char_t, lbound, ubound> &r)
    {
        return Trie_beta<char_t, lbound, ubound>::_equal(l, r);
    }

    template<typename char_t, char_t lbound, char_t ubound>
    bool operator != (const Trie_beta<char_t, lbound, ubound> &l, const Trie_beta<char_t, lbound, ubound> &r)
    {
        return Trie_beta<char_t, lbound, ubound>::_different(l, r);
    }

#undef ACHIBULUP__lval_fun
}


#endif // TRIE_H_INCLUDED
