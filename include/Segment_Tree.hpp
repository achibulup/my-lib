#ifndef SEGMENT_TREE_HPP_INCLUDED
#define SEGMENT_TREE_HPP_INCLUDED

#include <stdexcept>
#include <vector>
#include <string>
#include <climits>

namespace Achibulup
{
    using std::size_t;

    size_t size_pad(size_t x)
    {
        while(x & (x - 1)) x += x & -x;
        return x;
    }

    size_t count_trailing_zero(size_t x)
    {
        static const int tz[1 << 8] = {-1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
                                        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
        size_t res = 0;
        while(x >> 8)
          x >>= 8, res += 8;
        return res + tz[x];
    }

    template<typename Tp, size_t bits>
    struct mssb_helper;


    template<typename Tp>
    struct mssb_helper<Tp, 16>
    {
        static size_t get(Tp x)
        {
            x = (x >> 1) | (x >> 2);
            x |= x >> 2;
            x |= x >> 4;
            return (x | (x >> 8)) + 1;
        }
    };

    template<typename Tp>
    struct mssb_helper<Tp, 32>
    {
        static size_t get(Tp x)
        {
            x = (x >> 1) | (x >> 2);
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            return (x | (x >> 16)) + 1;
        }
    };

    template<typename Tp>
    struct mssb_helper<Tp, 64>
    {
        static size_t get(Tp x)
        {
            x = (x >> 1) | (x >> 2);
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            x |= x >> 16;
            return (x | (x >> 32)) + 1;
        }
    };


    inline size_t mssb(size_t x)
    {
        return mssb_helper<size_t, sizeof(size_t) * CHAR_BIT>::get(x);
    }


    inline size_t bin_tree_cut(size_t size)
    {
        if((size & (size - 1)) == 0) return size >> 1;
        size_t Mssb = mssb(size);
        return ((Mssb >> 1) & size) ? Mssb : (size - (Mssb >> 1));
    }

    template<typename Tp>
    std::string int2str(Tp val)
    {
        bool ne = 0;
        if(val < 0){
          ne = 1;
          val = -val;
        }
        std::string r, res;
        while(val > 0){
          r.push_back(val % 10 + '0');
          val /= 10;
        }
        res.reserve(ne + r.size());
        if(ne) res.push_back('-');
        for(std::string::size_type i = r.size(); i; --i)
          res.push_back(r[i - 1]);
        return res;
    }

    void throw_segtree_invalid_range(const char *query, ::size_t l, ::size_t r)
    {
        throw std::out_of_range(std::string(" Segment Tree ") + query + " query of invalid range : l (which is " +
                                int2str(l) + ") > r (which is " + int2str(r) + ")");
    }
    void throw_segtree_out_of_range(const char *query, const char *name, ::size_t size, ::size_t x)
    {
        throw std::out_of_range(std::string(" Segment Tree ") + query + " query out of range : " + name + " (which is " +
              int2str(x) + ") is not in range [0, size()) (which is [0, " + int2str(size) + "))");
    }
    void throw_segtree_out_of_range(const char *query, ::size_t size, ::size_t l, ::size_t r)
    {
        throw std::out_of_range(std::string(" Segment Tree ") + query + " query out of range : [l, r) (which is [" +
              int2str(l) + ", " + int2str(r) + ")) is not in range [0, size()) (which is [0, " + int2str(size) + "))");
    }










    template<typename answer_t, typename query_t, class functor>
    class Iterative_Segment_Tree
    {
    public:
        typedef ::size_t size_type;
        static const size_type npos = -1;

    private:
        const size_type m_size;
        const size_type padded_size;
        functor func;

        struct ans_answer_t
        {
            answer_t data;
            bool in_range;
        };
        std::vector<ans_answer_t> ans;

        struct lazy_answer_t
        {
            query_t data;
            bool init;
        };
        std::vector<lazy_answer_t> lazy;


        void init()
        {
            for(size_type i = padded_size; i < padded_size + m_size; ++i)
              ans[i].in_range = 1;

            const size_type depth = count_trailing_zero(padded_size) + 1;
            for(size_type d = depth - 1; d; --d){
              size_type chunk = depth - d;
              size_type lim = (m_size + (1 << (chunk - 1)) - 1) >> chunk;

              for(size_type i = 1 << (d - 1); i < (1 << (d - 1)) + lim; ++i){
                ans[i].in_range = 1;
                func.set(ans[i].data, func.combine(ans[i << 1].data, ans[i << 1 | 1].data));
              }

              if((lim << chunk) < m_size){
                ans[(1 << (d - 1)) + lim].in_range = 1;
                ans[(1 << (d - 1)) + lim].data = ans[((1 << (d - 1)) + lim) << 1].data;
              }
            }
        }

        void propagate(size_type i)
        {
            if(i < padded_size && lazy[i].init){
              func.apply(ans[i << 1].data, lazy[i].data);
              if((i << 1) < padded_size){
                if(lazy[i << 1].init)
                  func.add_up(lazy[i << 1].data, lazy[i].data);
                else{
                  lazy[i << 1].data = lazy[i].data;
                  lazy[i << 1].init = 1;
                }
              }

              if(ans[i << 1 | 1].in_range){
                func.apply(ans[i << 1 | 1].data, lazy[i].data);
                if((i << 1 | 1) < padded_size){
                  if(lazy[i << 1 | 1].init)
                    func.add_up(lazy[i << 1 | 1].data, lazy[i].data);
                  else{
                    lazy[i << 1 | 1].data = lazy[i].data;
                    lazy[i << 1 | 1].init = 1;
                  }
                }
              }

              lazy[i].init = 0;
            }
        }

        void push(size_type l, size_type r)
        {
            const size_type depth = count_trailing_zero(padded_size) + 1;
            size_type enl = 0, enr = 0, depl = 0, depr = 0;
            size_type d = depth;
            for(l += padded_size, r += padded_size; l != r; l >>= 1, r >>= 1){
              if(l & 1){
                if(depl == 0){
                  enl = l >> 1;
                  depl = d - 1;
                }
                ++l;
              }
              if(r & 1){
                if(depr == 0){
                  enr = r >> 1;
                  depr = d - 1;
                }
                --r;
              }
            }

            for(size_type cur = 0; depl; --depl){
              cur = cur << 1 | ((enl >> (depl - 1)) & 1);
              propagate(cur);
            }
            for(size_type cur = 0; depr; --depr){
              cur = cur << 1 | ((enr >> (depr - 1)) & 1);
              propagate(cur);
            }
        }

        void recalc(size_type l, size_type r)
        {
            size_type enl = 0, enr = 0;
            for(l += padded_size, r += padded_size; l != r; l >>= 1, r >>= 1){
              if(l & 1){
                if(enl == 0) enl = l;
                ++l;
              }
              if(r & 1){
                if(enr == 0) enr = r;
                --r;
              }
            }

            for(enl >>= 1; enl; enl >>= 1){
              if(ans[enl << 1 | 1].in_range)
                func.set(ans[enl].data, func.combine(ans[enl << 1].data, ans[enl << 1 | 1].data));
              else ans[enl].data = ans[enl << 1].data;
            }
            for(enr >>= 1; enr; enr >>= 1){
              if(ans[enr << 1 | 1].in_range)
                func.set(ans[enr].data, func.combine(ans[enr << 1].data, ans[enr << 1 | 1].data));
              else ans[enr].data = ans[enr << 1].data;
            }
        }

    public:
        explicit Iterative_Segment_Tree(size_type n) : m_size(n), padded_size(size_pad(m_size)), ans(padded_size * 2), lazy(padded_size)
        {
            init();
        }

        template<typename inp_iter>
        Iterative_Segment_Tree(inp_iter first, inp_iter last) : m_size(std::distance(first, last)),
                                                     padded_size(size_pad(m_size)), ans(padded_size * 2), lazy(padded_size)
        {
            for(size_type i = padded_size; i < padded_size + m_size; ++i){
              func.init(ans[i].data, *first);
              ++first;
            }
            init();
        }


        size_type size() const
        {
            return m_size;
        }


        const answer_t& update(size_type l, size_type r, const query_t& app)
        {
            if(l > r) throw_segtree_invalid_range("update", l, r);
            if(r > m_size) throw_segtree_out_of_range("update", m_size, l, r);
            if(l == r) return ans[1].data;

            push(l, r);

            for(size_type cl = l + padded_size, cr = r + padded_size; cl != cr; cl >>= 1, cr >>= 1){
              if(cl & 1){
                func.apply(ans[cl].data, app);
                if(cl < padded_size){
                  if(lazy[cl].init)
                    func.add_up(lazy[cl].data, app);
                  else{
                    lazy[cl].data = app;
                    lazy[cl].init = 1;
                  }

                  propagate(cl);
                }
                ++cl;
              }

              if(cr & 1){
                --cr;
                func.apply(ans[cr].data, app);
                if(cr < padded_size){
                  if(lazy[cr].init)
                    func.add_up(lazy[cr].data, app);
                  else{
                    lazy[cr].data = app;
                    lazy[cr].init = 1;
                  }

                  propagate(cr);
                }
              }
            }

            recalc(l, r);

            return ans[1].data;
        }

        const answer_t& update(size_type x, const query_t& app)
        {
            if(x >= m_size) throw_segtree_out_of_range("update", "x", m_size, x);

            x += padded_size;
            size_type d = count_trailing_zero(padded_size);
            for(size_type cur = 0; d; --d){
              cur = (cur << 1) | ((x >> d) & 1);
              propagate(cur);
            }

            func.apply(ans[x].data, app);

            for(x >>= 1; x; x >>= 1){
              if(ans[x << 1 | 1].in_range)
                func.set(ans[x].data, func.combine(ans[x << 1].data, ans[x << 1 | 1].data));
              else ans[x].data = ans[x << 1].data;
            }

            return ans[1].data;
        }

        answer_t get(size_type l, size_type r)
        {
            if(l > r) throw_segtree_invalid_range("get", l, r);
            if(r > m_size) throw_segtree_out_of_range("get", m_size, l, r);
            if(l == r) return answer_t();

            push(l, r);

            answer_t resl, resr;
            int il = 0, ir = 0;
            for(l += padded_size, r += padded_size; l != r; l >>= 1, r >>= 1){
              if(l & 1){
                if(il) resl = func.combine(resl, ans[l++].data);
                else resl = ans[l++].data;
                il = 1;
              }
              if(r & 1){
                if(ir) resr = func.combine(ans[--r].data, resr);
                else resr = ans[--r].data;
                ir = 1;
              }
            }

            if(!il) return resr;
            if(!ir) return resl;
            return func.combine(resl, resr);
        }

        const answer_t& get(size_type x)
        {
            if(x >= m_size) throw_segtree_out_of_range("get", "x", m_size, x);

            x += padded_size;
            size_type d = count_trailing_zero(padded_size);
            for(size_type cur = 0; d; --d){
              cur = (cur << 1) | ((x >> d) & 1);
              propagate(cur);
            }

            return ans[x].data;
        }

        const answer_t& get() const
        {
            return ans[1].data;
        }

        std::vector<answer_t> get_all()
        {
            std::vector<answer_t> res;
            res.reserve(m_size);

            const size_type depth = count_trailing_zero(padded_size) + 1;
            for(size_type d = 1; d < depth; ++d){
              size_type chunk = depth - d;
              size_type lim = (m_size + (1 << (chunk - 1)) - 1) >> chunk;

              for(size_type i = 1 << (d - 1); i < (1 << (d - 1)) + lim; ++i)
                propagate(i);
              if((lim << chunk) < m_size)
                propagate((1 << (d - 1)) + lim);
            }

            for(size_type i = padded_size; i < padded_size + m_size; ++i)
              res.emplace_back(ans[i].data);
            return res;
        }

        template<typename condition>
        size_type find(const condition &con, size_type start)
        {
            if(start >= m_size) throw_segtree_out_of_range("find", "start", m_size, start);

            push(start, m_size);

            answer_t cur;
            int ini = 0;
            size_type x = start + padded_size, l = start, r = start + 1;
            while(1){
              if(!ans[x].in_range) return npos;

              if(x & 1){
                if(ini){
                  answer_t tmp = func.combine(cur, ans[x].data);
                  if(func.satisfy(tmp, con)) break;
                  if(!(x & (x + 1))) return npos;
                  cur = tmp;
                }

                else{
                  if(func.satisfy(ans[x].data, con)) break;
                  if(!(x & (x + 1))) return npos;
                  cur = ans[x].data;
                  ini = 1;
                }

                ++x;
                r += r - l;
                l = (r + l) >> 1;
              }

              x >>= 1;
              r += r - l;
            }

            while(r - l > 1){
              propagate(x);

              if(ini){
                answer_t tmp = func.combine(cur, ans[x << 1].data);
                if(func.satisfy(tmp, con)){
                  r = (r + l) >> 1;
                  x = x << 1;
                }
                else{
                  cur = tmp;
                  l = (r + l) >> 1;
                  x = x << 1 | 1;
                }
              }

              else{
                if(func.satisfy(ans[x << 1].data, con)){
                  r = (r + l) >> 1;
                  x = x << 1;
                }
                else{
                  cur = ans[x << 1].data;
                  ini = 1;
                  l = (r + l) >> 1;
                  x = x << 1 | 1;
                }
              }
            }

            return l;
        }

        template<typename condition>
        size_type find(const condition &con)
        {
            return find(con, 0);
        }

        template<typename condition>
        size_type rfind(const condition &con, size_type rstart)
        {
            if(rstart >= m_size) throw_segtree_out_of_range("rfind", "rstart", m_size, rstart);

            push(0, rstart + 1);
            answer_t cur;
            int ini = 0;
            size_type x = rstart + padded_size, l = rstart, r = rstart + 1;
            while(1){
              if(x == 1 || !(x & 1)){
                if(ini){
                  answer_t tmp = func.combine(ans[x].data, cur);
                  if(func.satisfy(tmp, con)) break;
                  if(!(x & (x - 1))) return npos;
                  cur = tmp;
                }
                else{
                  if(func.satisfy(ans[x].data, con)) break;
                  if(!(x & (x - 1))) return npos;
                  cur = ans[x].data;
                  ini = 1;
                }

                --x;
                l -= r - l;
                r = (r + l) >> 1;
              }

              x >>= 1;
              l -= r - l;
            }

            while(r - l > 1){
              propagate(x);

              if(ini){
                answer_t tmp = func.combine(ans[x << 1 | 1].data, cur);
                if(func.satisfy(tmp, con)){
                  x = x << 1 | 1;
                  l = (r + l) >> 1;
                }
                else{
                  cur = tmp;
                  x = x << 1;
                  r = (r + l) >> 1;
                }
              }

              else{
                if(func.satisfy(ans[x << 1 | 1].data, con)){
                  x = x << 1 | 1;
                  l = (r + l) >> 1;
                }
                else{
                  cur = ans[x << 1 | 1].data;
                  ini = 1;
                  x = x << 1;
                  r = (r + l) >> 1;
                }
              }
            }

            return l;
        }

        template<typename condition>
        size_type rfind(const condition &con)
        {
            return rfind(con, m_size - 1);
        }
    };










    template<typename answer_t, typename query_t, class functor>
    class Recursive_Segment_Tree
    {
    public:
        typedef ::size_t size_type;
        static const size_type npos = -1;

    private:
        const size_type m_size;
        functor func;

        size_type curl, curr;
        answer_t cura;
        query_t curq;


        struct ans_answer_t
        {
            answer_t data;
        };
        std::vector<ans_answer_t> ans;

        struct lazy_answer_t
        {
            query_t data;
            bool init;
        };
        std::vector<lazy_answer_t> lazy;


        void init(size_type id, size_type left, size_type right)
        {
            if(right - left == 1) return;
            size_type mid = left + bin_tree_cut(right - left);
            init(id << 1, left, mid);
            init(id << 1 | 1, mid, right);
            func.set(ans[id].data, func.combine(ans[id << 1].data, ans[id << 1 | 1].data));
        }
        template<typename inp_iter>
        void init(size_type id, size_type left, size_type right, inp_iter &input)
        {
            if(right - left == 1) {
              func.init(ans[id].data, *input);
              ++input;
              return;
            }
            size_type mid = left + bin_tree_cut(right - left);
            init(id << 1, left, mid, input);
            init(id << 1 | 1, mid, right, input);
            func.set(ans[id].data, func.combine(ans[id << 1].data, ans[id << 1 | 1].data));
        }

        void propagate(size_type i, size_type len)
        {
            if(len > 1 && lazy[i].init){
              func.apply(ans[i << 1].data, lazy[i].data);
              if(len > 2){
                if(lazy[i << 1].init)
                  func.add_up(lazy[i << 1].data, lazy[i].data);
                else{
                  lazy[i << 1].data = lazy[i].data;
                  lazy[i << 1].init = 1;
                }
              }

              func.apply(ans[i << 1 | 1].data, lazy[i].data);
              if(len > 3){
                if(lazy[i << 1 | 1].init)
                  func.add_up(lazy[i << 1 | 1].data, lazy[i].data);
                else{
                  lazy[i << 1 | 1].data = lazy[i].data;
                  lazy[i << 1 | 1].init = 1;
                }
              }

              lazy[i].init = 0;
            }
        }

        void m_update(size_type id, size_type left, size_type right)
        {
            if(curl <= left && right <= curr){
              func.apply(ans[id].data, curq);
              if(right - left > 1){
                if(lazy[id].init)
                  func.add_up(lazy[id].data, curq);
                else{
                  lazy[id].data = curq;
                  lazy[id].init = 1;
                }
              }
              return;
            }
            propagate(id, right - left);
            size_type mid = left + bin_tree_cut(right - left);
            if(curl < mid) m_update(id << 1, left, mid);
            if(mid < curr) m_update(id << 1 | 1, mid, right);
            func.set(ans[id].data, func.combine(ans[id << 1].data, ans[id << 1 | 1].data));
        }

        answer_t m_get(size_type id, size_type left, size_type right)
        {
            if(curl <= left && right <= curr)
              return ans[id].data;
            propagate(id, right - left);
            size_type mid = left + bin_tree_cut(right - left);
            int il = curl < mid, ir = mid < curr;
            if(!il) return m_get(id << 1 | 1, mid, right);
            if(!ir) return m_get(id << 1, left, mid);
            return func.combine(m_get(id << 1, left, mid), m_get(id << 1 | 1, mid, right));
        }

        template<typename condition>
        size_type m_find(size_type id, size_type left, size_type right, const condition &con)
        {
            propagate(id, right - left);
            size_type mid = left + bin_tree_cut(right - left);
            if(curr){
              answer_t tmp = func.combine(cura, ans[id].data);
              if(!func.satisfy(tmp, con)){
                cura = tmp;
                return npos;
              }
              if(right - left == 1) return left;
              tmp = func.combine(cura, ans[id << 1].data);
              if(func.satisfy(tmp, con)) return m_find(id << 1, left, mid, con);
              cura = tmp;
              return m_find(id << 1 | 1, mid, right, con);
            }

            if(right - left == 1){
              if(func.satisfy(ans[id].data, con)) return left;
              cura = ans[id].data;
              curr = 1;
              return npos;
            }
            if(curl < mid){
              size_type cr = m_find(id << 1, left, mid, con);
              if(cr != npos) return cr;
            }
            return m_find(id << 1 | 1, mid, right, con);
        }

        template<typename condition>
        size_type m_rfind(size_type id, size_type left, size_type right, const condition &con)
        {
            propagate(id, right - left);
            size_type mid = left + bin_tree_cut(right - left);
            if(curl){
              answer_t tmp = func.combine(ans[id].data, cura);
              if(!func.satisfy(tmp, con)){
                cura = tmp;
                return npos;
              }
              if(right - left == 1) left;
              tmp = func.combine(ans[id << 1 | 1].data, cura);
              if(func.satisfy(tmp, con)) return m_rfind(id << 1 | 1, mid, right, con);
              cura = tmp;
              return m_rfind(id << 1, left, mid, con);
            }

            if(right - left == 1){
              if(func.satisfy(ans[id].data, con)) return left;
              cura = ans[id].data;
              curl = 1;
              return npos;
            }
            if(mid <= curr){
              size_type cr = m_rfind(id << 1 | 1, mid, right, con);
              if(cr != npos) return cr;
            }
            return m_rfind(id << 1, left, mid, con);
        }

        void collect(size_type id, size_type left, size_type right, std::vector<answer_t> &output)
        {
            if(right - left == 1){
              output.emplace_back(ans[id].data);
              return;
            }
            propagate(id, right - left);
            size_type mid = left + bin_tree_cut(right - left);
            collect(id << 1, left, mid, output);
            collect(id << 1 | 1, mid, right, output);
        }

    public:
        explicit Recursive_Segment_Tree(size_type n) : m_size(n), curq(), ans(m_size * 2), lazy(m_size)
        {
            init(1, 0, m_size);
        }

        template<typename inp_iter>
        Recursive_Segment_Tree(inp_iter first, inp_iter last) : m_size(std::distance(first, last)), curq(), ans(m_size * 2), lazy(m_size)
        {
            init(1, 0, m_size, first);
        }


        size_type size() const
        {
            return m_size;
        }


        const answer_t& update(size_type l, size_type r, const query_t& app)
        {
            if(l > r) throw_segtree_invalid_range("update", l, r);
            if(r > m_size) throw_segtree_out_of_range("update", m_size, l, r);
            if(l == r) return ans[1].data;

            curl = l, curr = r, curq = app;
            m_update(1, 0, m_size);

            return ans[1].data;
        }

        const answer_t& update(size_type x, const query_t& app)
        {
            return update(x, x + 1, app);
        }

        answer_t get(size_type l, size_type r)
        {
            if(l > r) throw_segtree_invalid_range("get", l, r);
            if(r > m_size) throw_segtree_out_of_range("get", m_size, l, r);
            if(l == r) return answer_t();

            curl = l;
            curr = r;

            return m_get(1, 0, m_size);
        }

        const answer_t& get(size_type x)
        {
            if(x >= m_size) throw_segtree_out_of_range("get", "x", m_size, x);

            size_type id = 1;
            for(size_type left = 0, right = m_size; right - left > 1;){
              propagate(id, right - left);
              size_type mid = left + bin_tree_cut(right - left);
              if(x < mid){
                id = id << 1;
                right = mid;
              }
              else {
                id = id << 1 | 1;
                left = mid;
              }
            }

            return ans[id].data;
        }

        const answer_t& get() const
        {
            return ans[1].data;
        }

        std::vector<answer_t> get_all()
        {
            std::vector<answer_t> res;
            res.reserve(m_size);
            collect(1, 0, m_size, res);
            return res;
        }

        template<typename condition>
        size_type find(const condition &con, size_type start)
        {
            if(start >= m_size) throw_segtree_out_of_range("find", "start", m_size, start);

            curl = start, curr = 0;
            return m_find(1, 0, m_size, con);
        }

        template<typename condition>
        size_type find(const condition &con)
        {
            return find(con, 0);
        }

        template<typename condition>
        size_type rfind(const condition &con, size_type rstart)
        {
            if(rstart >= m_size) throw_segtree_out_of_range("rfind", "rstart", m_size, rstart);

            curr = rstart, curl = 0;
            return m_rfind(1, 0, m_size, con);
        }

        template<typename condition>
        size_type rfind(const condition &con)
        {
            return rfind(con, m_size - 1);
        }
    };










    /**
        Segment tree data structure that supports ranged update queries and single get queries,
        not using lazy propagation.
    */
    template<class answer_t>
    class RUSG_Segment_Tree
    {
    public:
        typedef ::size_t size_type;
        static const size_type npos = -1;
        const size_type m_size;

    private:
        const size_type padded_size;
        std::vector<answer_t> ans;

        void init()
        {
            size_type depth = __builtin_ctz(padded_size) + 1;
            for(int d = depth - 2; d >= 0; --d){
              size_type chunk = depth - 1 - d;
              size_type lim = (m_size + (1 << (chunk - 1)) - 1) >> chunk;
              for(size_type i = (1 << d); i < (1 << d) + lim; ++i)
                ans[i].set(answer_t::combine(ans[i << 1], ans[i << 1 | 1]));
              if((lim << chunk) < m_size)
                ans[(1 << d) + lim] = ans[((1 << d) + lim) << 1];
            }
        }

    public:
        ///creating segment tree for n elements with answer_ts of default values
        RUSG_Segment_Tree(size_type sz) : m_size(sz), padded_size(size_pad(m_size)), ans(padded_size * 2)
        {
            init();
        }

        /**
          creating segment tree for a range of elements of type Tp, the leaf answer_ts are initialized with those elements
          to use this function, the answer_t type should contain a member function for the call :
          answer_t.init(Tp) : to initialize the answer_t with a value of type Tp
        */
        template<typename inp_iter>
        RUSG_Segment_Tree(inp_iter first, inp_iter last) : m_size(std::distance(first, last)), padded_size(size_pad(m_size)), ans(padded_size * 2)
        {
            for(size_type i = padded_size; i < padded_size + m_size; ++i){
              ans[i].init(*first);
              ++first;
            }
        }

        size_type size() const
        {
            return m_size;
        }

        /**
          updates the elements in range [l, r) by the object val
          throws an exception if the range is not inside the range [0, size)
          to call this funcion the answer_t type should contain a member function for the call
          answer_t.apply(query_t) : to update the answer_t with a value of type query_t
        */
        template<typename query_t>
        void update(size_type l, size_type r, const query_t& val)
        {
            if(l > r) throw_segtree_invalid_range("update", l, r);
            if(r > m_size) throw_segtree_out_of_range("update", m_size, l, r);

            for(l += padded_size, r += padded_size; l != r; l >>= 1, r >>= 1){
              if(l & 1)
                ans[l++].apply(val);
              if(r & 1)
                ans[r--].apply(val);
            }
        }

        template<typename query_t>
        void update(size_type x, const query_t& val)
        {
            if(x >= m_size) throw_segtree_out_of_range("update", "x", m_size, x);
            ans[x + padded_size].apply(val);
        }

        /**
          get the answer in the position x
          throws an exception if the range is not inside the range[0, size)

          The answer_t type should contain the member functions for the call :
          answer_t::combine(answer_t, answer_t) -> answer_t : to sum up the answers
        */
        answer_t get(size_type x) const
        {
            if(x >= m_size) throw_segtree_out_of_range("get", "x", m_size, x);

            x += padded_size;
            answer_t res = ans[x];
            for(x >>= 1; x; x >>= 1)
              res = answer_t::combine(res, ans[x]);
            return res;
        }
    };










    /**
        Segment tree data structure that supports single update queries and ranged get queries,
        not using lazy propagation.

        The answer_t type should contain the member functions for the call :

        answer_t::combine(answer_t, answer_t) -> answer_t : to get combine the answers in two consecutive range
        answer_t.set(answer_t) to update a answer_t in the tree from the above call to its two child

    */
    template<class answer_t>
    class SURG_Segment_Tree
    {
    public:
        typedef ::size_t size_type;
        static const size_type npos = -1;
        const size_type m_size;

    private:
        const size_type padded_size;
        std::vector<answer_t> ans;

        void init()
        {
            size_type depth = __builtin_ctz(padded_size) + 1;
            for(int d = depth - 2; d >= 0; --d){
              size_type chunk = depth - 1 - d;
              size_type lim = (m_size + (1 << (chunk - 1)) - 1) >> chunk;
              for(size_type i = (1 << d); i < (1 << d) + lim; ++i)
                ans[i].set(answer_t::combine(ans[i << 1], ans[i << 1 | 1]));
              if((lim << chunk) < m_size)
                ans[(1 << d) + lim] = ans[((1 << d) + lim) << 1];
            }
        }

    public:
        ///creating segment tree for n elements with answer_ts of default values
        SURG_Segment_Tree(size_type sz) : m_size(sz), padded_size(size_pad(m_size)), ans(padded_size * 2)
        {
            init();
        }

        /**
          creating segment tree for a range of elements of type Tp, the leaf answer_ts are initialized with those elements
          to use this function, the answer_t type should contain a member function for the call :
          answer_t.init(Tp) : to initialize the answer_t with a value of type Tp
        */
        template<typename inp_iter>
        SURG_Segment_Tree(inp_iter first, inp_iter last) : m_size(std::distance(first, last)), padded_size(size_pad(m_size)), ans(padded_size * 2)
        {
            for(size_type i = padded_size; i < padded_size + m_size; ++i){
              ans[i].init(*first);
              ++first;
            }
            init();
        }

        size_type size() const
        {
            return m_size;
        }

        /**
          updates the single element x by the object val
          throws an exception if x is not in range [0, size)
          to call this funcion the answer_t type should contain a member function for the call
          answer_t.apply(query_t) : to update the answer_t with a value of type query_t
        */
        template<typename query_t>
        const answer_t& update(size_type x, const query_t& val)
        {
            if(x >= m_size) throw_segtree_out_of_range("update", "x", m_size, x);

            size_type l = x - (x & 1), r = x + 1 + !(x & 1);
            x += padded_size;
            ans[x].apply(val);
            for(x >>= 1; x; x >>= 1){
              if((r + l) >> 1 < m_size) ans[x].set(answer_t::combine(ans[x << 1], ans[x << 1 | 1]));
              else ans[x] = ans[x << 1];
              if(x & 1) l -= r - l;
              else r += r - l;
            }

            return ans[1];
        }

        /**
          get the answer int the range [l, r)
          throws an exception if the range is not inside the range[0, size)
        */
        answer_t get(size_type l, size_type r) const
        {
            if(l > r) throw_segtree_invalid_range("get", l, r);
            if(r > m_size) throw_segtree_out_of_range("get", m_size, l, r);
            if(l == r) return answer_t();

            answer_t resl, resr;
            int il = 0, ir = 0;
            for(l += padded_size, r += padded_size; l != r; l >>= 1, r >>= 1){
              if(l & 1){
                if(il) resl = answer_t::combine(resl, ans[l++]);
                else resl = ans[l++];
                il = 1;
              }
              if(r & 1){
                if(ir) resr = answer_t::combine(ans[--r], resr);
                else resr = ans[--r];
                ir = 1;
              }
            }
            if(!il) return resr;
            if(!ir) return resl;
            return answer_t::combine(resl, resr);
        }

        /**
          get the answer to the element x in O(1)
          throws an exception if x is not in range [0, size)
        */
        const answer_t& get(size_type x) const
        {
            if(x >= m_size) throw_segtree_out_of_range("get", "x", m_size, x);
            return ans[x + padded_size];
        }

        /**
          get the answer to the full range in O(1)
        */
        const answer_t& get() const
        {
            return ans[1];
        }

        std::vector<answer_t> get_all() const
        {
            std::vector<answer_t> res;
            res.reserve(m_size);
            for(size_type i = padded_size; i < padded_size + m_size; ++i)
              res.emplace_back(ans[i]);
            return res;
        }

        /**
          find the first position pos starting from start that get(start, pos + 1) satisfy the condition con
          return the value npos if the position is not found
          to call this function the type answer_t should contain a member function for the call:
          answer_t::satisfy(answer_t, cond) -> bool : to check if the current answer satisfies the condition
        */
        template<typename cond>
        size_type find(const cond &con, size_type start) const
        {
            if(start >= m_size) throw_segtree_out_of_range("find", "start", m_size, start);
            size_type x = start + padded_size;
            size_type l = start, r = start + 1;
            answer_t sum;
            int ini = 0;
            for(;; x >>= 1, r += r - l){
              if(l >= m_size) return npos;
              if(x & 1){
                if(ini){
                  answer_t tmp = answer_t::combine(sum, ans[x]);
                  if(answer_t::satisfy(tmp, con)) break;
                  if(!(x & (x + 1))) return npos;
                  ++x;
                  sum = tmp;
                }
                else{
                  if(answer_t::satisfy(ans[x], con)) break;
                  if(!(x & (x + 1))) return npos;
                  sum = ans[x++];
                  ini = 1;
                }
                r += r - l;
                l = (r + l) >> 1;
              }
            }
            while(r - l > 1){
              if(ini){
                answer_t tmp = answer_t::combine(sum, ans[x << 1]);
                if(answer_t::satisfy(tmp, con)){
                  x = x << 1;
                  r = (r + l) >> 1;
                }
                else{
                  sum = tmp;
                  x = x << 1 | 1;
                  l = (r + l) >> 1;
                }
              }
              else{
                if(answer_t::satisfy(ans[x << 1], con)){
                  x = x << 1;
                  r = (r + l) >> 1;
                }
                else{
                  sum = ans[x << 1];
                  x = x << 1 | 1;
                  l = (r + l) >> 1;
                  ini = 1;
                }
              }
            }
            return l;
        }

        template<typename cond>
        size_type find(const cond &con) const
        {
            return find(con, 0);
        }

        /**
          similar to find but find the last position pos that get(pos, rstart + 1) satisfies the condition
        */
        template<typename cond>
        size_type rfind(const cond &con, size_type rstart) const
        {
            if(rstart >= m_size) throw_segtree_out_of_range("rfind", "rstart", m_size, rstart);
            size_type x = rstart + padded_size;
            size_type l = rstart, r = rstart + 1;
            answer_t sum;
            int ini = 0;
            for(;; x >>= 1, l -= r - l){
              if(x == 1 || !(x & 1)){
                if(ini){
                  answer_t tmp = answer_t::combine(ans[x], sum);
                  if(answer_t::satisfy(tmp, con)) break;
                  if(!(x & (x - 1))) return npos;
                  --x;
                  sum = tmp;
                }
                else{
                  if(answer_t::satisfy(ans[x], con)) break;
                  if(!(x & (x - 1))) return npos;
                  sum = ans[x--];
                  ini = 1;
                }
                l -= r - l;
                r = (r + l) >> 1;
              }
            }
            while(r - l > 1){
              if(ini){
                answer_t tmp = answer_t::combine(ans[x << 1 | 1], sum);
                if(answer_t::satisfy(tmp, con)){
                  x = x << 1 | 1;
                  l = (r + l) >> 1;
                }
                else{
                  sum = tmp;
                  x = x << 1;
                  r = (r + l) >> 1;
                }
              }
              else{
                if(answer_t::satisfy(ans[x << 1 | 1], con)){
                  x = x << 1 | 1;
                  l = (r + l) >> 1;
                }
                else{
                  sum = ans[x << 1 | 1];
                  x = x << 1;
                  r = (r + l) >> 1;
                  ini = 1;
                }
              }
            }
            return l;
        }

        template<typename cond>
        size_type rfind(const cond &con) const
        {
            return rfind(con, m_size - 1);
        }
    };

}

#endif // SEGMENT_TREE_HPP_INCLUDED
