#ifndef ACHIBULUP__FUNCTION_HPP_INCLUDED
#define ACHIBULUP__FUNCTION_HPP_INCLUDED
#include "common_utils.hpp"
#include <utility>
#include <memory>
namespace Achibulup
{
#if ACHIBULUP__Cpp14_later
    using std::make_unique;
#else
    template<typename Tp, typename ...Args>
    std::unique_ptr<Tp> make_unique(Args&&... args)
    {
        return std::unique_ptr<Tp>(new Tp(std::forward<Args>(args)...));
    }
#endif

    template<typename tp>
    class FunctorHolder;
    template<typename, typename>
    class HolderBase;
    ///a replicate of std::function
    template<typename Tp>
    class Function;

    template<typename Ret, typename ...Args>
    struct FunctorHolder<Ret(Args...)>
    {
        using pointer = std::unique_ptr<FunctorHolder>;

        template<typename Functor>
        static pointer make_functor(Functor &&functor)
        {
            using emplaced_type = typename std::decay<Functor>::type;
            return make_unique<HolderBase<emplaced_type, Ret(Args...)>>
                       (std::forward<Functor>(functor));
        }
        virtual ~FunctorHolder() = default;
        virtual Ret operator () (Args&&...) = 0;
        virtual pointer make_copy() = 0;
    };

    template<typename Functor, typename Ret, typename ...Args>
    class HolderBase<Functor, Ret(Args...)> 
    : public FunctorHolder<Ret(Args...)>
    {
        using base = FunctorHolder<Ret(Args...)>;

      public:
        HolderBase(const Functor &func) : functor(func) {}
        HolderBase(Functor &&func) : functor(std::move(func)) {}

        HolderBase(const HolderBase&) = delete;

        Ret operator () (Args&&... args) override
        {
            return static_cast<Ret>(functor(std::forward<Args>(args)...));
        }
        typename base::pointer make_copy() override
        {
            return make_unique<HolderBase>(functor);
        }

      private:
        Functor functor;
    };



    void throw_bad_function_call()
    {
        throw std::logic_error("calling empty function");
    }

    template<typename Ret, typename ...Args>
    class Function<Ret(Args...)>
    {
      private:
        using functor_base = FunctorHolder<Ret(Args...)>;
        template<typename Functor>
        using compatible = decltype(static_cast<Ret>(std::declval<Functor&>()
                                                      (std::declval<Args>()...)));

      public:
        constexpr Function() noexcept : i_fptr() {}

        Function(Ret(*fptr)(Args...)) 
        : i_fptr(functor_base::make_functor(fptr)) {}

        template<typename Functor, typename = compatible<Functor>>
        Function(Functor &&functor)
        : i_fptr(functor_base::make_functor(std::forward<Functor>(functor))) {}

        Function(Function &&mov) noexcept
        : i_fptr(Move(mov.i_fptr)) {}
        Function(const Function &cpy)
        : i_fptr(cpy.i_fptr->make_copy()) {}

        Function& operator = (Function asg) & noexcept
        {
            this->i_fptr = Move(asg.i_fptr);
        }

        explicit operator bool () const noexcept
        {
            return static_cast<bool>(this->i_fptr);
        }

        bool empty() const noexcept
        {
            return !*this;
        }

        Ret operator () (Args&&...args) const
        {
            if(!this) throw_bad_function_call();
            return (*this->i_fptr)(std::forward<Args>(args)...);
        }

      private:
        using base_pointer = typename functor_base::pointer;
        base_pointer i_fptr;
    };
    
}

#endif // ACHIBULUP__FUNCTION_HPP_INCLUDED

