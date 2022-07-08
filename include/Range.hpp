#ifndef RANGE_HPP_INCLUDED
#define RANGE_HPP_INCLUDED

#include "common_utils.hpp"
#include <type_traits>
#include <iterator>

namespace Achibulup
{

template<typename Iter, typename = void>
struct IterEnd
{
    using type = Iter;
};
template<typename Iter>
struct IterEnd<Iter, std::void_t<typename Iter::End>>
{
    using type = typename Iter::End;
};

template<typename Iter, typename End = typename IterEnd<Iter>::type>
class IterRange
{
  public:
    constexpr IterRange() noexcept = default;
    constexpr IterRange(Argument<Iter> begin, Argument<End> end = {}) noexcept 
    : m_begin(begin.get()), m_end(end.get()) {}

    template<typename Range,
             typename = decltype(std::begin(std::declval<Range>()))>
    constexpr IterRange(Range &&range) noexcept
    : m_begin(std::forward<Range>(range).begin()), 
      m_end(std::forward<Range>(range).end()) {}

    constexpr const Iter& begin() const noexcept
    {
        return this->m_begin;
    }
    constexpr const End& end() const noexcept
    {
        return this->m_end;
    }

    constexpr Iter&& releaseBegin() noexcept
    {
        return std::move(this->m_begin);
    }
    constexpr End&& releaseEnd() noexcept
    {
        return std::move(this->m_end);
    }

  private:
    Iter m_begin;
    End m_end;
};

template<typename Iter>
IterRange(const Iter&) -> IterRange<Iter>;
template<typename Iter, typename End>
IterRange(const Iter&, const End&) -> IterRange<Iter, End>;

template<typename Range,
         typename = decltype(std::begin(std::declval<Range>()))>
IterRange(Range &&) -> IterRange<decltype(std::begin(std::declval<Range>())), 
                                 decltype(std::end(std::declval<Range>()))>;
                              
template<typename Iter, typename End>
class InstantIterRange
{
  public:
    constexpr InstantIterRange() noexcept = default;
    constexpr InstantIterRange(Argument<Iter> begin, 
                               Argument<End> end = {}) noexcept
    : m_begin(begin.get()), m_end(end.get()) {}

    template<typename Range>
    constexpr InstantIterRange(Range &&range) noexcept
    : m_begin(std::forward<Range>(range).begin()), 
      m_end(std::forward<Range>(range).end()) {}

    constexpr Iter&& begin() noexcept
    {
        return std::move(this->m_begin);
    }
    constexpr End&& end() noexcept
    {
        return std::move(this->m_end);
    }
    constexpr const Iter&& begin() const noexcept
    {
        return std::move(this->m_begin);
    }
    constexpr const End&& end() const noexcept
    {
        return std::move(this->m_end);
    }

  private:
    Iter m_begin;
    End m_end;
};

template<typename Iter, typename End>
InstantIterRange<Iter, End> move(IterRange<Iter, End> &range) noexcept
{
    return InstantIterRange<Iter, End>(range.releaseBegin(),
                                       range.releaseEnd());
}
template<typename Iter, typename End>
InstantIterRange<Iter, End> move(IterRange<Iter, End> &&range) noexcept
{
    return move(range);
}

} // namespace Achibulup

#endif // RANGE_HPP_INCLUDED