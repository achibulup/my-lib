#ifndef SAFEUNION_H_INCLUDED
#define SAFEUNION_H_INCLUDED

// featuring type-safe union with named members

// usage example :

// #define NAMEOF(...) ACHIBULUP__NAMEOF(__VA_ARGS__)
// #define NAMEOF_T(...) ACHIBULUP__NAMEOF_T(__VA_ARGS__)
//     using Union = SafeUnion<
//       std::pair<int, NAMEOF_T(first)>,
//       std::pair<double, NAMEOF_T(second)>
//     >;
//     Union a(NAMEOF(first), 5), b(NAMEOF(second), 6);
//     b.get(NAMEOF(second)) = std::sqrt(10);
//     std::cout <<a.active_member()<<' '<<to_string(a)<<'\n'
//               <<b.active_member()<<' '<<to_string(b)<<'\n';
//     swap(a, b);
//     b.activate(NAMEOF(second), 1.2);
//     std::cout <<a.active_member()<<' '<<to_string(a)<<'\n'
//               <<b.active_member()<<' '<<to_string(b)<<'\n';

// output :

// first 5
// second 3.162278
// second 3.162278
// second 1.200000



#include <utility>
#include <type_traits>
#include "common_utils.h"
#include "string_literal.h"
#include "objectManagement.h"

namespace Achibulup
{
namespace n_helper
{
constexpr size_t max_int(size_t a, size_t b) noexcept
{
    return a > b ? a : b;
}
template<typename ...types>
constexpr typename std::enable_if<sizeof...(types) == 0, size_t>::type 
max_size() noexcept
{
    return 1;
}
template<typename Only, typename ...types>
constexpr typename std::enable_if<sizeof...(types) == 0, size_t>::type 
max_size() noexcept
{
    return sizeof(Only);
}
template<typename First, typename Second, typename ...Others>
constexpr size_t max_size() noexcept
{
    return max_int(max_int(sizeof(First), sizeof(Second)), max_size<Others...>());
}

template<typename ...types>
constexpr typename std::enable_if<sizeof...(types) == 0, size_t>::type 
max_align() noexcept
{
    return 1;
}
template<typename Only, typename ...types>
constexpr typename std::enable_if<sizeof...(types) == 0, size_t>::type 
max_align() noexcept
{
    return alignof(Only);
}
template<typename First, typename Second, typename ...Others>
constexpr size_t max_align() noexcept
{
    return max_int(max_int(alignof(First), alignof(Second)), max_align<Others...>());
}

void throw_inactive_member_exception(string_view access, string_view cur_active)
{
    if (cur_active[0] == '-')
      throw std::logic_error(string_format(
        "attempt to access inactive member : ",access," (the union is currently empty)"));
    throw std::logic_error(string_format(
        "attempt to access inactive member : ",access," (the current active member is : ",cur_active));
}

void throw_empty_union_format_exception()
{
    throw std::logic_error("attemp to format empty union");
}


template<typename Tp>
void CopyCtorFunc_helper(void* dest, const void* source, std::true_type)
{
    constructor<Tp>::copy_construct(1, static_cast<Tp*>(dest), 
                                    static_cast<const Tp*>(source));
}
template<typename Tp>
void CopyCtorFunc_helper(void* dest, const void* source, std::false_type)
{
    throw std::logic_error("type not copy constructible");
}
template<typename Tp>
void MoveCtorFunc_helper(void* dest, void* source, std::true_type)
{
    constructor<Tp>::move_construct(1, static_cast<Tp*>(dest), 
                                    static_cast<Tp*>(source));
}
template<typename Tp>
void MoveCtorFunc_helper(void* dest, void* source, std::false_type)
{
    throw std::logic_error("type not move constructible");
}

template<typename Tp>
void CopyCtorFunc(void* dest, const void* source)
{
    CopyCtorFunc_helper<Tp>(dest, source, std::is_copy_constructible<Tp>());
}
template<typename Tp>
void MoveCtorFunc(void* dest, void* source)
{
    MoveCtorFunc_helper<Tp>(dest, source, std::is_move_constructible<Tp>());
}
template<typename Tp>
void DtorFunc(void* ptr) noexcept
{
    Achibulup::destructor<Tp>::destroy(static_cast<Tp*>(ptr));
}
template<typename Tp>
auto to_stringFunc_helper(const Tp &val, int)
-> decltype(to_string(val))
{
    return to_string(val);
}
template<typename Tp>
std::string to_stringFunc_helper(const Tp&, short)
{
    throw std::logic_error("type not convertible to string");
}
template<typename Tp>
std::string to_stringFunc(const void *val)
{
    return to_stringFunc_helper(*Launder(static_cast<const Tp*>(val)), 0);
}


///*
struct TypeManagerStructure
{
    void (*copy_ctor)(void*, const void*);
    void (*move_ctor)(void*, void*);
    void (*dtor)(void*);
    std::string (*to_str)(const void*);
};

template<typename Tp>
struct TypeManager
{
    static constexpr TypeManagerStructure instance{
      CopyCtorFunc<Tp>, MoveCtorFunc<Tp>, DtorFunc<Tp>, to_stringFunc<Tp>
    };
};
template<typename Tp>
constexpr TypeManagerStructure TypeManager<Tp>::instance;

struct UnionMemberManagerStructure
{
    const TypeManagerStructure *manager;
    string_view name;
};

template<typename Type, typename Name>
struct UnionMemberManager
{
    static constexpr UnionMemberManagerStructure instance
    = {&TypeManager<Type>::instance, Name()};
};
template<typename Type, typename Name>
constexpr UnionMemberManagerStructure UnionMemberManager<Type, Name>::instance;



template<typename...>
struct SafeUnionParam{};

template<typename Name, typename firstType,
         typename ...Types, typename ...Names>
firstType constant_find(const SafeUnionParam<std::pair<firstType, Name>, 
                                                  std::pair<Types, Names>...>&);

template<typename Name, typename firstType, typename firstName,
         typename ...Types, typename ...Names>
auto constant_find(const SafeUnionParam<std::pair<firstType, firstName>,
                                             std::pair<Types, Names>...>&) 
-> decltype(constant_find<Name>(
      std::declval<const SafeUnionParam<std::pair<Types, Names>...>&>()));

template<typename Name, typename Union>
using Union_find_t = decltype(constant_find<Name>(
    std::declval<const Union&>()));


constexpr bool ands() noexcept 
{
    return true;
}
template<typename ...Bool>
constexpr bool ands(bool first, Bool&& ...others) noexcept
{
    return first && ands(std::forward<Bool>(others)...);
}
constexpr bool ors() noexcept
{
    return false;
}
template<typename ...Bool>
constexpr bool ors(bool first, Bool&& ...others) noexcept
{
    return first || ors(std::forward<Bool>(others)...);
}


template<typename ...Types, typename ...Names>
constexpr bool are_trivially_destructible(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_trivially_destructible<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_trivially_copyable(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_trivially_copyable<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_copy_constructible(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_copy_constructible<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_move_constructible(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_move_constructible<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_copy_assignable(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_copy_assignable<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_move_assignable(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_move_assignable<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_assignable(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands((std::is_copy_assignable<Types>::value 
              || std::is_move_assignable<Types>::value)...);
}


template<typename ...Types, typename ...Names>
constexpr bool are_nothrow_copy_constructible(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_nothrow_copy_constructible<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_nothrow_move_constructible(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_nothrow_move_constructible<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_nothrow_copy_assignable(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_nothrow_copy_assignable<Types>::value...);
}
template<typename ...Types, typename ...Names>
constexpr bool are_nothrow_move_assignable(
  SafeUnionParam<std::pair<Types, Names>...>) noexcept
{
    return ands(std::is_nothrow_move_assignable<Types>::value...);
}




template<typename>
struct SafeUnion_data_base;

template<typename Params, 
         typename = std::integral_constant<
                      bool, are_trivially_destructible(Params{})
                    >
        >
struct SafeUnion_traits0;

template<typename Params, 
         typename = std::integral_constant<
                      bool, are_trivially_copyable(Params{})
                    >
        >
struct SafeUnion_traits1;


template<typename ...Types, typename ...Names>
struct SafeUnion_data_base<SafeUnionParam<std::pair<Types, Names>...>> 
{
    using Params = SafeUnionParam<std::pair<Types, Names>...>;

    template<typename Name>
    using lookup = Union_find_t<Name, Params>;
    
    using manager_t = const UnionMemberManagerStructure*;
    template<typename Name>
    using MemberManager = UnionMemberManager<lookup<Name>, Name>;

    SafeUnion_data_base() noexcept : current() {}

    bool empty() const noexcept
    {
        return !this->current;
    }

    template<typename Name>
    void reset_current() noexcept
    {
        this->current = &MemberManager<Name>::instance;
    }

    void reset_current() noexcept
    {
        this->current = {};
    }

    alignas(max_align<Types...>()) 
    byte storage[max_size<Types...>()];
    manager_t current;
};

template<typename ...Types, typename ...Names>
struct SafeUnion_traits0<
         SafeUnionParam<std::pair<Types, Names>...>, 
         std::true_type
       >
: SafeUnion_data_base<SafeUnionParam<std::pair<Types, Names>...>>
{
    void destruct() noexcept {}
};

template<typename ...Types, typename ...Names>
struct SafeUnion_traits0<
         SafeUnionParam<std::pair<Types, Names>...>, 
         std::false_type
       >
: SafeUnion_data_base<SafeUnionParam<std::pair<Types, Names>...>>
{
    ~SafeUnion_traits0()
    {
        this->destruct();
    }
    void destruct() noexcept
    {
        this->current->manager->dtor(this->storage);
    }
};

template<typename ...Types, typename ...Names>
struct SafeUnion_traits1<
         SafeUnionParam<std::pair<Types, Names>...>, 
         std::true_type
       >
: SafeUnion_traits0<SafeUnionParam<std::pair<Types, Names>...>>
{
    void reset() noexcept
    {
        this->reset_current();
    }
};

template<typename ...Types, typename ...Names>
struct SafeUnion_traits1<
         SafeUnionParam<std::pair<Types, Names>...>, 
         std::false_type
       >
: SafeUnion_traits0<SafeUnionParam<std::pair<Types, Names>...>>
{
    using Params = SafeUnionParam<std::pair<Types, Names>...>;

    SafeUnion_traits1() noexcept = default;

    SafeUnion_traits1(const SafeUnion_traits1 &other)
    noexcept(are_nothrow_copy_constructible(Params{}))
    : SafeUnion_traits1()
    {
        static_assert(are_copy_constructible(Params{}),
                      "all union member must be copy constructible");
        this->copy_assign(other);
    }
    SafeUnion_traits1(SafeUnion_traits1 &&other)
    noexcept(are_nothrow_move_constructible(Params{}))
    : SafeUnion_traits1()
    {
        static_assert(are_move_constructible(Params{}),
                      "all union member must be move constructible");
        this->move_assign(other);
    }

    SafeUnion_traits1& operator = (const SafeUnion_traits1 &other) &
    noexcept(are_nothrow_copy_assignable(Params{}))
    {
        static_assert(are_copy_assignable(Params{}),
                      "all union member must be copy assignable");
        this->reset();
        this->copy_assign(other);
        return *this;
    }
    SafeUnion_traits1& operator = (SafeUnion_traits1 &&other) &
    noexcept(are_nothrow_move_assignable(Params{}))
    {
        static_assert(are_move_assignable(Params{}),
                      "all union member must be move assignable");
        this->reset();
        this->move_assign(other);
        return *this;
    }

    void reset() noexcept
    {
        if (!this->empty()) {
          this->destruct();
          this->reset_current();
        }
    }

    void copy_assign(const SafeUnion_traits1 &other)
    {
        if (!other.empty())
          other.current->manager->copy_ctor(this->storage, other.storage);
        this->current = other.current;
    }
    void move_assign(SafeUnion_traits1 &other)
    {
        if (!other.empty())
          other.current->manager->move_ctor(this->storage, other.storage);
        this->current = other.current;
    }
};


} // n_helper


template<typename...>
struct SafeUnion;

template<typename ...Types, typename ...Names>
class SafeUnion<std::pair<Types, Names>...> 
: private n_helper::SafeUnion_traits1<
            n_helper::SafeUnionParam<std::pair<Types, Names>...>
          >
{
    using Params = n_helper::SafeUnionParam<std::pair<Types, Names>...>;
    using direct_base = n_helper::SafeUnion_traits1<Params>;

  public:
    template<typename Name>
    using lookup = typename direct_base::template lookup<Name>;

    SafeUnion() noexcept = default;

    template<typename Name, typename ...Args,
             typename Type = lookup<Name>>
    SafeUnion(Name, Args&& ...args) 
    noexcept(noexcept(Type(std::forward<Args>(args)...)))
    : SafeUnion()
    {
        this->emplace<Name>(std::forward<Args>(args)...);
    }

    /// to deal with the case that a brace initializer is passed in
    template<typename Name>
    SafeUnion(Name, lookup<Name> &&val) 
    noexcept(std::is_nothrow_move_constructible<lookup<Name>>::value)
    : SafeUnion()
    {   
        using Type = lookup<Name>;
        this->emplace<Name>(std::forward<Type>(val));
    }

    SafeUnion(const SafeUnion&) = default;
    SafeUnion(SafeUnion &&) = default;
    
    SafeUnion& operator = (const SafeUnion &) & = default;
    SafeUnion& operator = (SafeUnion &&) & = default;

    friend void swap(SafeUnion &a, SafeUnion &b) 
    noexcept(noexcept(std::swap(a, b)))
    {
        std::swap(a, b);
    }


    static constexpr size_t member_count() noexcept
    {
        return sizeof...(Types);
    }

    using direct_base::empty;

    template<typename Name,
             typename present = lookup<Name>>
    bool is_active(Name) const noexcept
    {
        return this->current == &MemberManager<Name>::instance;
    }

    string_view active_member() const noexcept
    {
        if (!this->current) return "--";
        return this->current->name;
    }


    template<typename Name, typename ...Args, 
             typename Type = lookup<Name>>
    Type& activate(Name, Args&& ...args) &
    noexcept(noexcept(Type(std::forward<Args>(args)...)))
    {
        static_assert(n_helper::are_assignable(Params{}),
                      "all members must be assignable");
        this->emplace<Name>(std::forward<Args>(args)...);
        return this->get(Name());
    }
    template<typename Name, typename Type = lookup<Name>>
    Type& activate(Name, lookup<Name> &&val) &
    noexcept(std::is_nothrow_move_constructible<lookup<Name>>::value)
    {
        static_assert(n_helper::are_assignable(Params{}),
                      "all members must be assignable");
        this->emplace<Name>(std::forward<Type>(val));
        return this->get(Name());
    }


    SafeUnion& reset() & noexcept
    {
        static_assert(n_helper::are_assignable(Params{}),
                      "all members must be assignable");
        direct_base::reset();
        return *this;
    }

    template<typename Name, 
             typename result_type = lookup<Name>>
    result_type& get(Name) &
    {
        this->check_active<Name>();
        return *reinterpret_cast<result_type*>(this->storage);
    }
    template<typename Name, 
             typename result_type = lookup<Name>>
    const result_type& get(Name) const &
    {
        this->check_active<Name>();
        return *reinterpret_cast<const result_type*>(this->storage);
    }
    template<typename Name, 
             typename result_type = lookup<Name>>
    result_type&& get(Name) &&
    {
        return std::move(this->get(Name()));
    }
    template<typename Name, 
             typename result_type = lookup<Name>>
    const result_type&& get(Name) const &&
    {
        return std::move(this->get(Name()));
    }

    friend std::string to_string(const SafeUnion &uni)
    {
        return uni.to_string();
    }

  private:
    template<typename Name>
    using MemberManager = typename direct_base::template MemberManager<Name>;

    template<typename Name, typename ...Args>
    void emplace(Args&& ...args)
    {
        using Type = lookup<Name>;
        this->reset();
        constructor<Type>::construct(reinterpret_cast<Type*>(this->storage),
                                     std::forward<Args>(args)...);
        this->template reset_current<Name>();
    }

    using direct_base::destruct;

    std::string to_string() const
    {
        if (this->empty()) 
          n_helper::throw_empty_union_format_exception();
        return this->current->manager->to_str(this->storage);
    }

    template<typename Name>
    void check_active()
    {
        if (!this->is_active(Name())) 
          n_helper::throw_inactive_member_exception(Name(), active_member());
    }

    using direct_base::reset_current;
};

}
#endif // SAFEUNION_H_INCLUDED