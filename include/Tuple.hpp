#ifndef TUPLE_HPP_INCLUDED
#define TUPLE_HPP_INCLUDED
#include <type_traits>
#include <utility>
#include "Type_utils.h"
#include "common_utils.hpp"

namespace Achibulup
{
#if ACHIBULUP__Cpp17_later

    template<size_t ...seq>
    struct size_t_sequence{};

    template<typename, size_t>
    struct add_to_size_t_sequence;
    template<size_t ...seq, size_t add>
    struct add_to_size_t_sequence<size_t_sequence<seq...>, add>
    {
        using type = size_t_sequence<seq..., add>;
    };

    template<size_t len>
    struct index_sequence
    {
        using type = typename add_to_size_t_sequence<typename index_sequence<len - 1>::type, len - 1>::type;
    };
    template<>
    struct index_sequence<0>
    {
        using type = size_t_sequence<>;
    };


    template<typename Tp, size_t index>
    struct Tuple_element
    {
        using value_type = Tp;
        static constexpr size_t number = index;
        value_type value;

        constexpr Tuple_element(const Tuple_element&) = default;
        constexpr Tuple_element(Tuple_element&&) = default;

        constexpr void operator = (const Tuple_element& rhs) &
        {   value = rhs.value;   }
        constexpr void operator = (Tuple_element&& rhs) &
        {   value = static_cast<value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element&& rhs) &
        {   value = static_cast<const value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element& rhs) const &
        {   value = rhs.value;   }
        constexpr void operator = (Tuple_element&& rhs) const &
        {   value = static_cast<value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element&& rhs) const &
        {   value = static_cast<const value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element& rhs) &&
        {   static_cast<value_type&&>(value) = rhs.value;   }
        constexpr void operator = (Tuple_element&& rhs) &&
        {   static_cast<value_type&&>(value) = static_cast<value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element&& rhs) &&
        {   static_cast<value_type&&>(value) = static_cast<const value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element& rhs) const &&
        {   static_cast<const value_type&&>(value) = rhs.value;   }
        constexpr void operator = (Tuple_element&& rhs) const &&
        {   static_cast<const value_type&&>(value) = static_cast<value_type&&>(rhs.value);   }
        constexpr void operator = (const Tuple_element&& rhs) const &&
        {   static_cast<const value_type&&>(value) = static_cast<const value_type&&>(rhs.value);   }

        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>& rhs) &
        {   value = rhs.value;   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (Tuple_element<_Tp, _index>&& rhs) &
        {   value = static_cast<typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>&& rhs) &
        {   value = static_cast<const typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>& rhs) const &
        {   value = rhs.value;   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (Tuple_element<_Tp, _index>&& rhs) const &
        {   value = static_cast<typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>&& rhs) const &
        {   value = static_cast<const typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>& rhs) &&
        {   static_cast<value_type&&>(value) = rhs.value;   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (Tuple_element<_Tp, _index>&& rhs) &&
        {   static_cast<value_type&&>(value) = static_cast<typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>&& rhs) &&
        {   static_cast<value_type&&>(value) = static_cast<const typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>& rhs) const &&
        {   static_cast<const value_type&&>(value) = rhs.value;   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (Tuple_element<_Tp, _index>&& rhs) const &&
        {   static_cast<const value_type&&>(value) = static_cast<typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
        template<typename _Tp, size_t _index>
        constexpr void operator = (const Tuple_element<_Tp, _index>&& rhs) const &&
        {   static_cast<const value_type&&>(value) = static_cast<const typename Tuple_element<_Tp, _index>::value_type&&>(rhs.value);   }
    };

    template<typename Tp, size_t Nm, size_t index>
    struct Tuple_element<Tp[Nm], index>
    {
        using value_type = Tp[Nm];
        static constexpr size_t number = index;
        value_type value;
    };



    template<typename, typename>
    struct Tuple_helper;

    template<typename ...Tp>
    struct Tuple : public Tuple_helper<typename_pack<Tp...>, typename index_sequence<sizeof...(Tp)>::type>;

    template<typename ...Tp, size_t ...seq>
    struct Tuple_helper<typename_pack<Tp...>, size_t_sequence<seq...>> : public Tuple_element<Tp, seq>...
    {
        template<size_t idx>
        using ith_element = typename typename_pack<Tp...>::template ith_type<idx>;

        template<size_t idx>
        using ith_base = Tuple_element<ith_element<idx>, idx>;

        static constexpr size_t element_count = sizeof...(Tp);


        //constexpr Tuple_helper(const Tuple_helper&) = default;
        //constexpr Tuple_helper(Tuple_helper&&) = default;


        ///get the ith element of the Tuple
        template<size_t index, typename std::enable_if<index < element_count>::type* = nullptr>
        constexpr ith_element<index>& get() &
        {
            return ith_base<index>::value;
        }
        template<size_t index, typename std::enable_if<index < element_count>::type* = nullptr>
        constexpr const ith_element<index>& get() const &
        {
            return ith_base<index>::value;
        }
        template<size_t index, typename std::enable_if<index < element_count>::type* = nullptr>
        constexpr ith_element<index>&& get() &&
        {
            return static_cast<ith_element<index>&&>(ith_base<index>::value);
        }
        template<size_t index, typename std::enable_if<index < element_count>::type* = nullptr>
        constexpr const ith_element<index>&& get() const &&
        {
            return static_cast<const ith_element<index>&&>(ith_base<index>::value);
        }

        template<typename _Tp, typename std::enable_if<type_count<_Tp, typename_pack<Tp...>>::value == 1>::type* = nullptr>
        constexpr _Tp& get() &
        {
            return get<type_find<_Tp, typename_pack<Tp...>>::value>();
        }
        template<typename _Tp, typename std::enable_if<type_count<_Tp, typename_pack<Tp...>>::value == 1>::type* = nullptr>
        constexpr const _Tp& get() const &
        {
            return get<type_find<_Tp, typename_pack<Tp...>>::value>();
        }
        template<typename _Tp, typename std::enable_if<type_count<_Tp, typename_pack<Tp...>>::value == 1>::type* = nullptr>
        constexpr _Tp&& get() &&
        {
            return static_cast<_Tp&&>(get<type_find<_Tp, typename_pack<Tp...>>::value>());
        }
        template<typename _Tp, typename std::enable_if<type_count<_Tp, typename_pack<Tp...>>::value == 1>::type* = nullptr>
        constexpr const _Tp&& get() const &&
        {
            return static_cast<const _Tp&&>(get<type_find<_Tp, typename_pack<Tp...>>::value>());
        }

        ///assigning Tuples, equivalent to assigning each of its element
        ///for elements of reference type, assignments are invoked on refered objects
        constexpr Tuple_helper& operator = (const Tuple_helper &rhs) &
        {
            ((static_cast<ith_base<seq>&>(*this) = static_cast<const ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        constexpr Tuple_helper& operator = (Tuple_helper &&rhs) &
        {
            ((static_cast<ith_base<seq>&>(*this) = static_cast<ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr Tuple_helper& operator = (const Tuple_helper &&rhs) &
        {
            ((static_cast<ith_base<seq>&>(*this) = static_cast<const ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr const Tuple_helper& operator = (const Tuple_helper &rhs) const &
        {
            ((static_cast<const ith_base<seq>&>(*this) = static_cast<const ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        constexpr const Tuple_helper& operator = (Tuple_helper &&rhs) const &
        {
            ((static_cast<const ith_base<seq>&>(*this) = static_cast<ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr const Tuple_helper& operator = (const Tuple_helper &&rhs) const &
        {
            ((static_cast<const ith_base<seq>&>(*this) = static_cast<const ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr Tuple_helper&& operator = (const Tuple_helper &rhs) &&
        {
            ((static_cast<ith_base<seq>&&>(*this) = static_cast<const ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        constexpr Tuple_helper&& operator = (Tuple_helper &&rhs) &&
        {
            ((static_cast<ith_base<seq>&&>(*this) = static_cast<ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr Tuple_helper&& operator = (const Tuple_helper &&rhs) &&
        {
            ((static_cast<ith_base<seq>&&>(*this) = static_cast<const ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr const Tuple_helper&& operator = (const Tuple_helper &rhs) const &&
        {
            ((static_cast<const ith_base<seq>&&>(*this) = static_cast<const ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        constexpr const Tuple_helper&& operator = (Tuple_helper &&rhs) const &&
        {
            ((static_cast<const ith_base<seq>&&>(*this) = static_cast<ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        constexpr const Tuple_helper&& operator = (const Tuple_helper &&rhs) const &&
        {
            ((static_cast<const ith_base<seq>&&>(*this) = static_cast<const ith_base<seq>&&>(rhs)), ...);
            return *this;
        }


        ///assigning to different Tuples of same element count
        ///equivalent to assigning each of its element
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr Tuple_helper& operator = (const Tuple_helper<_Tp, _seq> &rhs) &
        {
            ((static_cast<ith_base<seq>&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr Tuple_helper& operator = (Tuple_helper<_Tp, _seq> &&rhs) &
        {
            ((static_cast<ith_base<seq>&>(*this) = static_cast<typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr Tuple_helper& operator = (const Tuple_helper<_Tp, _seq> &&rhs) &
        {
            ((static_cast<ith_base<seq>&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr const Tuple_helper& operator = (const Tuple_helper<_Tp, _seq> &rhs) const &
        {
            ((static_cast<const ith_base<seq>&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr const Tuple_helper& operator = (Tuple_helper<_Tp, _seq> &&rhs) const &
        {
            ((static_cast<const ith_base<seq>&>(*this) = static_cast<typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr const Tuple_helper& operator = (const Tuple_helper<_Tp, _seq> &&rhs) const &
        {
            ((static_cast<const ith_base<seq>&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr Tuple_helper&& operator = (const Tuple_helper<_Tp, _seq> &rhs) &&
        {
            ((static_cast<ith_base<seq>&&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr Tuple_helper&& operator = (Tuple_helper<_Tp, _seq> &&rhs) &&
        {
            ((static_cast<ith_base<seq>&&>(*this) = static_cast<typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr Tuple_helper&& operator = (const Tuple_helper<_Tp, _seq> &&rhs) &&
        {
            ((static_cast<ith_base<seq>&&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr const Tuple_helper&& operator = (const Tuple_helper<_Tp, _seq> &rhs) const &&
        {
            ((static_cast<const ith_base<seq>&&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr const Tuple_helper&& operator = (Tuple_helper<_Tp, _seq> &&rhs) const &&
        {
            ((static_cast<const ith_base<seq>&&>(*this) = static_cast<typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr const Tuple_helper&& operator = (const Tuple_helper<_Tp, _seq> &&rhs) const &&
        {
            ((static_cast<const ith_base<seq>&&>(*this) = static_cast<const typename Tuple_helper<_Tp, _seq>::template ith_base<seq>&&>(rhs)), ...);
            return *this;
        }


        ///converts to an Tuple of the same element count, using corresponding conversions on each element
        ///use conversion operators instead of constructors to keep Tuple an aggregate
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr operator Tuple_helper<_Tp, _seq> () &
        {
            return {Tuple_element<Tp, seq>::value...};
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr operator Tuple_helper<_Tp, _seq> () const &
        {
            return {Tuple_element<Tp, seq>::value...};
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr operator Tuple_helper<_Tp, _seq> () &&
        {
            return {static_cast<ith_element<seq>&&>(Tuple_element<Tp, seq>::value)...};
        }
        template<typename _Tp, typename _seq,
        typename std::enable_if<Tuple_helper<_Tp, _seq>::element_count == element_count>::type* = nullptr>
        constexpr operator Tuple_helper<_Tp, _seq> () const &&
        {
            return {static_cast<const ith_element<seq>&&>(Tuple_element<Tp, seq>::value)...};
        }



        ///creating an Tuple holding rvalue reference to its members
        ///equivalent to std::move-ing each of its member
        constexpr Tuple<moved_type<Tp>...> move ()
        {
            return {std::move(ith_base<seq>::value)...};
        }
        constexpr Tuple<moved_type<Tp>...> move () const
        {
            return {std::move(ith_base<seq>::value)...};
        }

    };

#else

     ///a replicate of std::tuple
    template<typename...>
    struct Tuple;


    ///a support for Tuple::get
    template<size_t>
    struct int_deducer{};
    template<typename, typename>
    struct ith_member;
    template<size_t index, typename ...Tps>
    struct ith_member<int_deducer<index>, Tuple<Tps...>>
    : public ith_member<int_deducer<index>, expand_pack_t<Tuple, typename_pack_pop_back_t<typename_pack<Tps...>>>>
    {
        using _base = ith_member<int_deducer<index>, expand_pack_t<Tuple, typename_pack_pop_back_t<typename_pack<Tps...>>>>;
        static inline constexpr auto get(Tuple<Tps...> &tup) -> decltype(_base::get(tup.fval))
        {
            return _base::get(tup.fval);
        }
        static inline constexpr auto get(const Tuple<Tps...> &tup) -> decltype(_base::get(tup.fval))
        {
            return _base::get(tup.fval);
        }
    };
    template<typename ...Tps>
    struct ith_member<int_deducer<sizeof...(Tps) - 1>, Tuple<Tps...>>
    {
        using base = Tuple<Tps...>;
        using type = typename typename_pack<Tps...>::last_type;
        static inline constexpr decltype(Tuple<Tps...>::sval)& get(Tuple<Tps...> &tup)
        {
            return tup.sval;
        }
        static inline constexpr const decltype(Tuple<Tps...>::sval)& get(const Tuple<Tps...> &tup)
        {
            return tup.sval;
        }
    };

    template<typename from, typename to>
    using exp_conv_success = decltype(void(static_cast<to>(std::declval<from>())));
    template<typename from, typename to>
    using imp_conv_success = decltype(std::declval<void(to)>()(std::declval<from>()));

    ///workaround to a compiler bug
    template<typename Tp>
    using rvalref = Tp&&;




    template<typename ...Tps>
    struct Tuple
    {
        using firsts = expand_pack_t<Tuple, typename_pack_pop_back_t<typename_pack<Tps...>>>;
        using last = typename typename_pack<Tps...>::last_type;

        template<size_t idx>
        using ith_base = typename ith_member<int_deducer<idx>, Tuple>::base;

        template<size_t idx>
        using ith_element = typename ith_member<int_deducer<idx>, Tuple>::type;

        static constexpr size_t element_count = sizeof...(Tps);



        firsts fval;
        last sval;


        //constexpr Tuple(const Tuple&) = default;
        //constexpr Tuple(Tuple&&) = default;

        ///get the ith member of the Tuple
        template<size_t index, typename std::enable_if<index < sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 ith_element<index>& get() &
        {
            return ith_member<int_deducer<index>, Tuple>::get(*this);
        }
        template<size_t index, typename std::enable_if<index < sizeof...(Tps)>::type* = nullptr>
        constexpr const ith_element<index>& get() const &
        {
            return ith_member<int_deducer<index>, Tuple>::get(*this);
        }
        template<size_t index, typename std::enable_if<index < sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 ith_element<index>&& get() &&
        {
            return static_cast<rvalref<ith_element<index>>>(get<index>());
        }
        template<size_t index, typename std::enable_if<index < sizeof...(Tps)>::type* = nullptr>
        constexpr const ith_element<index>&& get() const &&
        {
            return static_cast<rvalref<const ith_element<index>>>(get<index>());
        }

        template<typename Tp, typename std::enable_if<type_count<Tp, typename_pack<Tps...>>::value == 1>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 Tp& get() &
        {
            return get<type_find<Tp, typename_pack<Tps...>>::value>();
        }
        template<typename Tp, typename std::enable_if<type_count<Tp, typename_pack<Tps...>>::value == 1>::type* = nullptr>
        constexpr const Tp& get() const &
        {
            return get<type_find<Tp, typename_pack<Tps...>>::value>();
        }
        template<typename Tp, typename std::enable_if<type_count<Tp, typename_pack<Tps...>>::value == 1>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 Tp&& get() &&
        {
            return static_cast<rvalref<Tp>>(get<type_find<Tp, typename_pack<Tps...>>::value>());
        }
        template<typename Tp, typename std::enable_if<type_count<Tp, typename_pack<Tps...>>::value == 1>::type* = nullptr>
        constexpr const Tp&& get() const &&
        {
            return static_cast<rvalref<const Tp>>(get<type_find<Tp, typename_pack<Tps...>>::value>());
        }

        ///assigning Tuples, equivalent to assigning each of its member
        ///for members of reference type, assignments are invoked on refered objects
        ACHIBULUP__constexpr_fun14 Tuple& operator = (const Tuple &cpy) &
        {
            fval = cpy.fval;
            sval = cpy.sval;
            return *this;
        }
        ACHIBULUP__constexpr_fun14 Tuple& operator = (Tuple &&mov) &
        {
            fval = static_cast<firsts&&>(mov.fval);
            sval = static_cast<last&&>(mov.sval);
            return *this;
        }
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (const Tuple &cpy) const &
        {
            fval = cpy.fval;
            sval = cpy.sval;
            return *this;
        }
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (Tuple &&mov) const &
        {
            fval = static_cast<firsts&&>(mov.fval);
            sval = static_cast<last&&>(mov.sval);
            return *this;
        }
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (const Tuple &cpy) &&
        {
            static_cast<firsts&&>(fval) = cpy.fval;
            static_cast<rvalref<last>>(sval) = cpy.sval;
            return static_cast<Tuple&&>(*this);
        }
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (Tuple &&mov) &&
        {
            static_cast<firsts&&>(fval) = static_cast<firsts&&>(mov.fval);
            static_cast<rvalref<last>>(sval) = static_cast<last&&>(mov.sval);
            return static_cast<Tuple&&>(*this);
        }
        constexpr const Tuple&& operator = (const Tuple &cpy) const &&
        {
            static_cast<const firsts&&>(fval) = cpy.fval;
            static_cast<rvalref<const last>>(sval) = cpy.sval;
            return static_cast<Tuple&&>(*this);
        }
        constexpr const Tuple&& operator = (Tuple &&mov) const &&
        {
            static_cast<const firsts&&>(fval) = static_cast<firsts&&>(mov.fval);
            static_cast<rvalref<const last>>(sval) = static_cast<last&&>(mov.sval);
            return static_cast<Tuple&&>(*this);
        }


        ///assigning to different Tuples of same member count
        ///equivalent to assigning each of its member
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 Tuple& operator = (const Tuple<_Tps...> &cpy) &
        {
            fval = cpy.fval;
            sval = cpy.sval;
            return *this;
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 Tuple& operator = (Tuple<_Tps...> &&mov) &
        {
            fval = static_cast<typename Tuple<_Tps...>::firsts&&>(mov.fval);
            sval = static_cast<typename Tuple<_Tps...>::last&&>(mov.sval);
            return *this;
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (const Tuple<_Tps...> &cpy) const &
        {
            fval = cpy.fval;
            sval = cpy.sval;
            return *this;
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (Tuple<_Tps...> &&mov) const &
        {
            fval = static_cast<typename Tuple<_Tps...>::firsts&&>(mov.fval);
            sval = static_cast<typename Tuple<_Tps...>::last&&>(mov.sval);
            return *this;
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (const Tuple<_Tps...> &cpy) &&
        {
            static_cast<firsts&&>(fval) = cpy.fval;
            static_cast<rvalref<last>>(sval) = cpy.sval;
            return static_cast<Tuple&&>(*this);
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (Tuple<_Tps...> &&mov) &&
        {
            static_cast<firsts&&>(fval) = static_cast<typename Tuple<_Tps...>::firsts&&>(mov.fval);
            static_cast<rvalref<last>>(sval) = static_cast<typename Tuple<_Tps...>::last&&>(mov.sval);
            return static_cast<Tuple&&>(*this);
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        constexpr const Tuple&& operator = (const Tuple<_Tps...> &cpy) const &&
        {
            static_cast<const firsts&&>(fval) = cpy.fval;
            static_cast<rvalref<const last>>(sval) = cpy.sval;
            return static_cast<Tuple&&>(*this);
        }
        template<typename ..._Tps, typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        constexpr const Tuple&& operator = (Tuple<_Tps...> &&mov) const &&
        {
            static_cast<const firsts&&>(fval) = static_cast<typename Tuple<_Tps...>::firsts&&>(mov.fval);
            static_cast<rvalref<const last>>(sval) = static_cast<typename Tuple<_Tps...>::last&&>(mov.sval);
            return static_cast<Tuple&&>(*this);
        }


        ///converts to an Tuple of the same member count, using corresponding conversions on each members
        ///use conversion operators instead of constructors to keep Tuple an aggregate
        template<typename ..._Tps,
        typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        explicit ACHIBULUP__constexpr_fun14 operator Tuple<_Tps...> () &
        {
            return {static_cast<typename Tuple<_Tps...>::firsts>(fval),
                    static_cast<typename Tuple<_Tps...>::last>(sval)};
        }
        template<typename ..._Tps,
        typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        explicit constexpr operator Tuple<_Tps...> () const &
        {
            return {static_cast<typename Tuple<_Tps...>::firsts>(fval),
                    static_cast<typename Tuple<_Tps...>::last>(sval)};
        }
        template<typename ..._Tps,
        typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        explicit ACHIBULUP__constexpr_fun14 operator Tuple<_Tps...> () &&
        {
            return {static_cast<typename Tuple<_Tps...>::firsts>(static_cast<firsts&&>(fval)),
                    static_cast<typename Tuple<_Tps...>::last>(static_cast<rvalref<last>>(sval))};
        }
        template<typename ..._Tps,
        typename std::enable_if<sizeof...(_Tps) == sizeof...(Tps)>::type* = nullptr>
        explicit constexpr operator Tuple<_Tps...> () const &&
        {
            return {static_cast<typename Tuple<_Tps...>::firsts>(static_cast<firsts&&>(fval)),
                    static_cast<typename Tuple<_Tps...>::last>(static_cast<rvalref<const last>>(sval))};
        }



        ///creating an Tuple holding rvalue reference to its members
        ///equivalent to std::move-ing each of its member
        ACHIBULUP__constexpr_fun14 Tuple<moved_type<Tps>...> move ()
        {
            return {fval.move(), std::move(sval)};
        }
        constexpr Tuple<moved_type<const Tps>...> move () const
        {
            return {fval.move(), std::move(sval)};
        }

    };


    ///specialization for one member
    template<typename only>
    struct Tuple<only>
    {
        using last = only;

        template<size_t idx>
        using ith_base = typename ith_member<int_deducer<idx>, Tuple>::base;

        template<size_t idx>
        using ith_element = typename ith_member<int_deducer<idx>, Tuple>::type;

        static constexpr size_t element_count = 1;


        last sval;



        constexpr Tuple(const Tuple&) = default;
        constexpr Tuple(Tuple&&) = default;



        template<size_t index, typename std::enable_if<index == 0>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 only& get() &
        {
            return sval;
        }
        template<size_t index, typename std::enable_if<index == 0>::type* = nullptr>
        constexpr const only& get() const &
        {
            return sval;
        }
        template<size_t index, typename std::enable_if<index == 0>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 only&& get() &&
        {
            return static_cast<rvalref<last>>(sval);
        }
        template<size_t index, typename std::enable_if<index == 0>::type* = nullptr>
        constexpr const only&& get() const &&
        {
            return static_cast<rvalref<const last>>(sval);
        }

        template<typename Tp, typename std::enable_if<std::is_same<Tp, only>::value>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 only& get() &
        {
            return sval;
        }
        template<typename Tp, typename std::enable_if<std::is_same<Tp, only>::value>::type* = nullptr>
        constexpr const only& get() const &
        {
            return sval;
        }
        template<typename Tp, typename std::enable_if<std::is_same<Tp, only>::value>::type* = nullptr>
        ACHIBULUP__constexpr_fun14 only&& get() &&
        {
            return static_cast<rvalref<last>>(sval);
        }
        template<typename Tp, typename std::enable_if<std::is_same<Tp, only>::value>::type* = nullptr>
        constexpr const only&& get() const &&
        {
            return static_cast<rvalref<const last>>(sval);
        }


        ACHIBULUP__constexpr_fun14 Tuple& operator = (const Tuple& cpy) &
        {
            sval = cpy.sval;
            return *this;
        }
        ACHIBULUP__constexpr_fun14 Tuple& operator = (Tuple&& mov) &
        {
            sval = static_cast<last&&>(mov.sval);
            return *this;
        }
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (const Tuple& cpy) const &
        {
            sval = cpy.sval;
            return *this;
        }
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (Tuple&& mov) const &
        {
            sval = static_cast<last&&>(mov.sval);
            return *this;
        }
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (const Tuple& cpy) &&
        {
            static_cast<rvalref<last>>(sval) = cpy.sval;
            return static_cast<Tuple&&>(*this);
        }
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (Tuple&& mov) &&
        {
            static_cast<rvalref<last>>(sval) = static_cast<last&&>(mov.sval);
            return static_cast<Tuple&&>(*this);
        }
        ACHIBULUP__constexpr_fun14 const Tuple&& operator = (const Tuple& cpy) const &&
        {
            static_cast<rvalref<const last>>(sval) = cpy.sval;
            return static_cast<const Tuple&&>(*this);
        }
        ACHIBULUP__constexpr_fun14 const Tuple&& operator = (Tuple&& mov) const &&
        {
            static_cast<rvalref<const last>>(sval) = static_cast<last&&>(mov.sval);
            return static_cast<const Tuple&&>(*this);
        }

        template<typename Tp>
        ACHIBULUP__constexpr_fun14 Tuple& operator = (const Tuple<Tp>& cpy) &
        {
            sval = cpy.sval;
            return *this;
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 Tuple& operator = (Tuple<Tp>&& mov) &
        {
            sval = static_cast<Tp&&>(mov.sval);
            return *this;
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (const Tuple<Tp>& cpy) const &
        {
            sval = cpy.sval;
            return *this;
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 const Tuple& operator = (Tuple<Tp>&& mov) const &
        {
            sval = static_cast<Tp&&>(mov.sval);
            return *this;
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (const Tuple<Tp>& cpy) &&
        {
            static_cast<rvalref<last>>(sval) = cpy.sval;
            return static_cast<Tuple&&>(*this);
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 Tuple&& operator = (Tuple<Tp>&& mov) &&
        {
            static_cast<rvalref<last>>(sval) = static_cast<Tp&&>(mov.sval);
            return static_cast<Tuple&&>(*this);
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 const Tuple&& operator = (const Tuple<Tp>& cpy) const &&
        {
            static_cast<rvalref<const last>>(sval) = cpy.sval;
            return static_cast<const Tuple&&>(*this);
        }
        template<typename Tp>
        ACHIBULUP__constexpr_fun14 const Tuple&& operator = (Tuple<Tp>&& mov) const &&
        {
            static_cast<rvalref<const last>>(sval) = static_cast<Tp&&>(mov.sval);
            return static_cast<const Tuple&&>(*this);
        }



        template<typename Tp>
        explicit ACHIBULUP__constexpr_fun14 operator Tuple<Tp> () &
        {
            return {static_cast<Tp>(sval)};
        }

        template<typename Tp>
        explicit constexpr operator Tuple<Tp> () const &
        {
            return {static_cast<Tp>(sval)};
        }

        template<typename Tp>
        explicit ACHIBULUP__constexpr_fun14 operator Tuple<Tp> () &&
        {
            return {static_cast<Tp>(static_cast<rvalref<last>>(sval))};
        }

        template<typename Tp>
        explicit constexpr operator Tuple<Tp> () const &&
        {
            return {static_cast<Tp>(static_cast<rvalref<const last>>(sval))};
        }



        ACHIBULUP__constexpr_fun14 Tuple<moved_type<last>> move ()
        {
            return {std::move(sval)};
        }
        constexpr Tuple<moved_type<const last>> move () const
        {
            return {std::move(sval)};
        }
    };
    template<>
    struct Tuple<>
    {
        static constexpr size_t element_count = 0;

        ACHIBULUP__constexpr_fun14 Tuple move ()
        {
            return {};
        }

        constexpr const Tuple move () const
        {
            return {};
        }
    };

#undef ACHIBULUP__constexpr_fun14

#endif // __cplusplus

    template<typename ...Tp>
    constexpr Tuple<typename std::decay<Tp>::type...> make_Tuple(Tp&& ...val)
    {
        return {static_cast<Tp&&>(val)...};
    }


    ///create an Tuple holding references to corresponding arguments
    ///usually used for assigning multiple objects at once
    template<typename ...types>
    constexpr Tuple<types&&...> tie(types&& ...args)
    {
        return {static_cast<types&&>(args)...};
    }

    struct ignore_t
    {
        constexpr ignore_t() = default;
        template<typename Tp>
        constexpr ignore_t(Tp&&) {}
        template<typename Tp>
        constexpr const ignore_t& operator = (Tp&&) const {return *this;}
    };
    constexpr ignore_t ignore = ignore_t();
}
#endif // TUPLE_HPP_INCLUDED