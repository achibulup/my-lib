#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED
#include <climits>
#include <vector>
#include <string>
#include <cstring>
#include <initializer_list>
#include "common_utils.h"

namespace Achibulup
{

    template<char lbound = 32, char ubound = 127>
    class Trie
    {
        using id_type = std::int32_t;

      public:
        using size_type = std::int32_t;
        static const char char_lower_bound = lbound;
        static const char char_upper_bound = ubound;
        static const size_type char_range = ubound - lbound + 1;

        class cursor
        {
            cursor(id_type val) : i_value(val) {}

          public:
            using hash_type = std::size_t;

            cursor() : i_value(0) {}

            hash_type hash() const
            {
                return i_value;
            }

            friend bool operator == (const cursor &l, const cursor &r)
            {
                return l.i_value == r.i_value;
            }
            friend bool operator != (const cursor &l, const cursor &r)
            {
                return l.i_value != r.i_value;
            }
          
          private:
            id_type i_value;
            friend Trie;
        };
    
      private:
        struct node
        {
            node(id_type par, id_type idx, char val = 0) 
            : id(idx), parent_id(par), value(val), 
              ecnt(0), pcnt(0), child() {}

            const id_type id;
            id_type parent_id;
            char value;
            size_type ecnt;
            size_type pcnt;
            id_type child[char_range];

            bool is_word() const
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

            const node* go_to(id_type _id) const
            {
                return this + (_id - id);
            }

            id_type next(char val) const
            {
                id_type res = child[val - lbound];
                if(res == nposid) return nposid;
                const node *pos = go_to(res);
                return (pos->pcnt == 0) ? nposid : res;
            }

            size_type prefix_length() const
            {
                size_type res = 0;
                const node *cur = this;
                while(cur->id != rootid){
                  ++res;
                  cur = cur->go_to(cur->parent_id);
                }
                return res;
            }

            std::string get_prefix() const
            {
                std::string res;
                if(id != nposid){
                  size_type dep = prefix_length();
                  res.resize(dep);
                  const node *cur = this;
                  while(dep--){
                    res[dep] = cur->value;
                  cur = cur->go_to(cur->parent_id);
                  }
                }
                return res;
            }

        };

        class data_t
        {
          protected:
            data_t() : base(), str() {}
            data_t(const node &_base) 
            : str(_base.get_prefix()), base(&_base) {}

          public:
            std::string str;

            bool is_word() const
            {
                return base->is_word();
            }

            size_type as_end_count() const
            {
                return base->as_end_count();
            }

            size_type as_prefix_count() const
            {
                return base->as_prefix_count();
            }

        protected:
            const node *base;
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
            const_iterator(const node &nod) : data_t(nod) {}

            const_iterator& operator ++()
            {
                char curv = lbound;
                bool ok = false;

                while(ok == false){
                  while(1){
                    id_type pos = data_t::base->child[curv - lbound];
                    if(pos != nposid){
                      const node *ptr = data_t::base->go_to(pos);
                      if(ptr->pcnt != 0){
                        data_t::base = ptr;
                        data_t::str.push_back(curv);
                        ok = true;
                        break;
                      }
                    }

                    if(curv == ubound) break;
                    ++curv;
                  }

                  if(ok) break;

                  while(data_t::base->id != rootid && data_t::base->value == ubound){
                    data_t::base = data_t::base->go_to(data_t::base->parent_id);
                    data_t::str.pop_back();
                  }
                  if(data_t::base->id == rootid){
                    data_t::base = data_t::base->go_to(nposid);
                    ok = true;
                    break;
                  }
                  curv = data_t::base->value + 1;
                  data_t::base = data_t::base->go_to(data_t::base->parent_id);
                  data_t::str.pop_back();
                }

                return *this;
            }
            const_iterator& operator -- ()
            {
                char curv;
                if(data_t::base->index != nposid){
                  curv = data_t::base->value;
                  data_t::base = data_t::base->go_to(data_t::base->parent_id);
                  data_t::str.pop_back();
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
                        data_t::str.push_back(curv);
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
        using iterator = const_iterator;
        

        Trie() : i_data(), i_reuse(), i_ncnt(0), i_wcnt(0)
        {
            do_init();
        }

        Trie(std::initializer_list<string_view> init_list) : Trie()
        {
            for(string_view elem : init_list)
              insert(elem);
        }

        bool empty() const
        {
            return i_wcnt == 0;
        }

        size_type node_count() const
        {
            return i_ncnt;
        }

        size_type word_count() const
        {
            return i_wcnt;
        }

        void reserve(size_type count) &
        {
            i_data.reserve(count);
        }

        friend bool operator == (const Trie &l, const Trie &r)
        {
            if(&l == &r) return true;
            return do_comp_tree(l.i_data[rootid], r.i_data[rootid]);
        }
        friend bool operator !=(const Trie &l, const Trie &r)
        {
            return !(l == r);
        }

        friend std::string to_string(const Trie &dict, char delim = '\n')
        {
            std::string res;
            for(auto &&entry : dict)
              if (entry.is_word()) {
                if (!res.empty()) res.push_back(delim);
                res += entry.str;
              }
            return res;
        }
        friend std::string to_string(const Trie &dict, std::string delim)
        {
            std::string res;
            for(auto &&entry : dict)
              if (entry.is_word()) {
                if (!res.empty()) res += delim;
                res += entry.str;
              }
            return res;
        }

        bool is_word(const cursor &pos) const
        {
            return do_at(pos).is_word();
        }
        size_type as_end_count(const cursor &pos) const
        {
            return do_at(pos).as_end_count();
        }
        size_type as_prefix_count(const cursor &pos) const
        {
            return do_at(pos).as_prefix_count();
        }

        cursor prev(const cursor &pos) const
        {
            return cursor(do_at(pos).parent_id);
        }

        cursor next(const cursor &pos, char val) const
        {
            return cursor(do_at(pos).next(val));
        }

        size_type prefix_length(const cursor &pos) const
        {
            return do_at(pos).prefix_length();
        }

        std::string get_prefix(const cursor &pos) const
        {
            return do_at(pos).get_prefix();
        }

        cursor insert(const char *str, size_type len) &
        {
            return insert({str, len});
        }
        cursor insert(string_view str) &
        {
            cursor cur = do_force_root();
            for(char c : str)
              cur = do_next_or_new_node(cur, c);
            
            cursor res = cur;
            if(!do_at(res).ecnt){
              do_at(res).ecnt = 1;
              ++i_wcnt;
              while(cur != root()){
                if (!do_at(cur).pcnt++) ++i_ncnt;
                cur = prev(cur);
              }
            }
            return res;
        }


        cursor find(const char *str, size_type len) const
        {
            return find({str, len});
        }
        cursor find(string_view str) const
        {
            cursor res = find_prefix(str);
            if (is_word(res)) return res;
            return npos();
        }

        cursor find_prefix(const char *str, size_type len) const
        {
            return find_prefix({str, len});
        }
        cursor find_prefix(string_view str) const
        {
            if (empty()) return npos();
            cursor cur = root();
            for(char c : str)
              if ((cur = next(cur, c)) == npos()) 
                return npos();
            return cur;
        }

        size_type count(const char *str, size_type len) const
        {
            return count({str,len});
        }
        size_type count(string_view str) const
        {
            return find(str) != npos() ? 1 : 0;
        }
        bool contains(const char *str, size_type len) const
        {
            return contains({str,len});
        }
        bool contains(string_view str) const
        {
            return count(str);
        }

        bool erase(cursor pos) &
        {
            if(do_at(pos).ecnt == 0) return false;
            do_at(pos).ecnt = 0;
            --i_wcnt;
            while(pos != root()){
              do_decrease_prefix_count(pos);
              pos = prev(pos);
            }
            do_decrease_prefix_count(root());
            return true;
        }
        bool erase(const char *str, size_type len) &
        {
            return erase({str, len});
        }
        bool erase(string_view str) &
        {
            cursor pos = find(str);
            return pos != npos() && erase(pos);
        }

        cursor root() const
        {
            if (empty()) return npos();
            return cursor(rootid);
        }

        cursor npos() const
        {
            return cursor(nposid);
        }

        const_iterator cbegin() const
        {
            return make_const_iterator(root());
        }
        iterator begin() const
        {
            return cbegin();
        }
        const_iterator cend() const
        {
            return make_const_iterator(npos());
        }
        iterator end() const
        {
            return cend();
        }

        const_iterator make_const_iterator(const cursor &cs) const
        {
            return const_iterator(do_at(cs));
        }
        iterator make_iterator(const cursor &cs) const
        {
            return const_iterator(do_at(cs));
        }

      private:
        static const id_type rootid = 1;
        static const id_type nposid = 0;

        node& do_get_node(id_type id) &
        {
            return i_data[id];
        }
        const node& do_get_node(id_type id) const &
        {
            return i_data[id];
        }

        node& do_at(const cursor &pos) &
        {
            return do_get_node(do_get_id(pos));
        }
        const node& do_at(const cursor &pos) const &
        {
            return do_get_node(do_get_id(pos));
        }

        static id_type do_get_id(const cursor &pos)
        {
            return pos.i_value;
        }

        void do_init() &
        {
            i_data.clear();
            i_data.emplace_back(nposid, nposid);
            i_data.emplace_back(nposid, rootid);
        }

        cursor do_new_node(cursor parent, char val)
        {
            if(i_reuse.empty()){
              id_type id = i_data.size();
              i_data.push_back(node(do_get_id(parent), id, val));
              ++i_ncnt;
              return cursor(id);
            }
            
            id_type id = i_reuse.back();
            i_data[id].parent_id = do_get_id(parent);
            i_data[id].value = val;
            i_reuse.pop_back();
            ++i_ncnt;
            return cursor(id);
        }

        cursor do_next_or_new_node(const cursor &pos, char c) &
        {
            cursor res = next(pos, c);
            if (res == npos()) {
              res = do_new_node(pos, c);
              do_at(pos).child[c - lbound] = do_get_id(res);
            }
            return res;
        }

        cursor do_force_root() &
        {
            if (empty()) {
              ++i_ncnt;
              ++do_get_node(rootid).pcnt;
            }
            return cursor(rootid);
        }

        void do_decrease_prefix_count(const cursor &pos) &
        {
            if (!--do_at(pos).pcnt)
              do_recycle(pos);
        }
        void do_recycle(const cursor &pos) &
        {
            if (pos != root()) {
              i_reuse.push_back(do_get_id(pos));
              do_at(prev(pos)).child[do_at(pos).value - lbound] = nposid;
            }
            --i_ncnt;
        }

        static bool do_comp_tree(const node &l, const node &r)
        {
            if(l.ecnt != r.ecnt)
              return false;
            if(l.pcnt != r.pcnt)
              return false;
            for(size_type i = 0; i < char_range; ++i){
              if ((l.child[i] == nposid) != (r.child[i] == nposid))
                return false;
              else if ((l.child[i] != nposid)
                && (!do_comp_tree(*l.go_to(l.child[i]), *r.go_to(r.child[i]))))
                  return false;
            }
            return true;
        }



        std::vector<node> i_data;
        std::vector<id_type> i_reuse;
        size_type i_ncnt;
        size_type i_wcnt;
    };
    template<char l, char u>
    const size_t Trie<l, u>::rootid;
    template<char l, char u>
    const size_t Trie<l, u>::nposid;





/*
    template<typename char_t, char_t lbound = 32, char_t ubound = 127>
    class Trie_beta
    {

    public:
        typedef char_t char_type;
        typedef size_t size_type;
        static const char_t char_lower_bound = lbound;
        static const char_t char_upper_bound = ubound;

    private:
        struct Trie_access{};
        static const size_type char_range = static_cast<size_type>(ubound - lbound) + 1;
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
            node *child[char_range];

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

            std::basic_string<char_t> get_prefix() const
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
        size_type i_ncnt;
        size_type i_wcnt;

        node* get_next(node *parent, char_t val)
        {
            ++i_ncnt;
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
            for(size_type i = 0; i < char_range; ++i)
              if(sour->child[i]){
                dest->child[i] = new node(dest, lbound + i);
                copy_tree(dest->child[i], sour->child[i]);
              }
        }
        static void destroy_tree(node *cur)
        {
            for(size_type i = 0; i < char_range; ++i)
              if(cur->child[i]) destroy_tree(cur->child[i]);
            delete cur;
        }
        static void assign_tree(node *dest, const node *sour)
        {
            dest->ecnt = sour->ecnt;
            dest->pcnt = sour->pcnt;
            for(size_type i = 0; i < char_range; ++i){
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
            for(size_type i = 0; i < char_range; ++i){
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

        Trie_beta() : _root(new node()), i_ncnt(0), i_wcnt(0)
        {
            init();
        }

        Trie_beta(const Trie_beta &cpy) : _root(new node()), i_ncnt(cpy.i_ncnt), i_wcnt(cpy.i_wcnt)
        {
            init();
            copy_tree(_root, cpy._root);
        }

#if __cplusplus >= 201103L
        Trie_beta(Trie_beta &&mov) : _root(mov._root), i_ncnt(mov.i_ncnt), i_wcnt(mov.i_wcnt)
        {
            mov._root = new node();
            mov.init();
        }
#endif // __cplusplus

        Trie_beta& operator = (const Trie_beta &cpy) &
        {
            if(&cpy != this){
              i_ncnt = cpy.i_ncnt;
              i_wcnt = cpy.i_wcnt;
              assign_tree(_root, cpy._root);
            }
            return *this;
        }

#if __cplusplus >= 201103L
        Trie_beta& operator = (Trie_beta &&mov) &
        {
            if(&mov != this){
              size_type tn = i_ncnt;
              i_ncnt = mov.i_ncnt;
              mov.i_ncnt = tn;
              size_type tw = i_wcnt;
              i_wcnt = mov.i_wcnt;
              mov.i_wcnt = tw;
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
            return i_ncnt;
        }

        size_type word_count() const
        {
            return i_wcnt;
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

        std::basic_string<char_t> get_prefix(const cursor &cs) const
        {
            return get(cs)->get_prefix();
        }

        cursor insert(const char_t *str, size_type len) &
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
              if(_root->pcnt == 0) ++i_ncnt;
              cur->ecnt = true;
              ++i_wcnt;
              while(1){
                ++(cur->pcnt);
                if(cur == _root) break;
                cur = cur->parent.index;
              }
            }
            return to_cursor(res);
        }
        cursor insert(string_view<char_t> strv) &
        {
            return insert(strv.ptr, strv.len);
        }

#if __cplusplus >= 201103
        Trie_beta(std::initializer_list<string_view<char_t>> init_list) : _root(new node()), i_ncnt(0), i_wcnt(0)
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

        bool erase(const cursor &cs) &
        {
            node *pos = get(cs);
            if(pos->ecnt == 0) return false;
            pos->ecnt = 0;
            --i_wcnt;
            while(1){
              --(pos->pcnt);
              if(pos->pcnt == 0){
                --i_ncnt;
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
        bool erase(const char *str, size_type len) &
        {
            cursor cs = find(str, len);
            return cs != npos() && erase(cs);
        }
        bool erase(string_view<char_t> strv) &
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
            data_t(const node *_base, Trie_access) : base(_base), word(_base->get_prefix()) {}

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
*/
}


#endif // TRIE_H_INCLUDED
