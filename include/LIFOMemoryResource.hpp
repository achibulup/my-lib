#ifndef LIFOMemoryResource_HPP_INCLUDED
#define LIFOMemoryResource_HPP_INCLUDED

#include "common_utils.hpp"
#include "objectManagement.hpp"
#include <vector>
#include <cstddef>


#if ACHIBULUP__Cpp17_later

#if __has_include(<memory_resource>)
#include <memory_resource>
#define ACHIBULUP__have_memory_resource 1
#define ACHIBULUP__MEMORY_RESOURCE : public std::pmr::memory_resource
#else
#define ACHIBULUP__have_memory_resource 0
#define ACHIBULUP__MEMORY_RESOURCE
#endif // __has_include(<memory_resource>)

#else
#define ACHIBULUP__have_memory_resource 0
#define ACHIBULUP__MEMORY_RESOURCE
#endif // ACHIBULUP__Cpp17_later
namespace Achibulup
{

class GrowingBufferListStrategy
{
  public:
    /// create an empty memory resouce that currently has no memory allocated
    /// and an implementation-defined growth factor
    GrowingBufferListStrategy() noexcept;

    GrowingBufferListStrategy(const GrowingBufferListStrategy&) = delete;
    GrowingBufferListStrategy(GrowingBufferListStrategy&&) noexcept;
    
    void operator = (const GrowingBufferListStrategy&) = delete;
    GrowingBufferListStrategy& 
    operator = (GrowingBufferListStrategy&&) noexcept;
    
    ~GrowingBufferListStrategy();

    void swap(GrowingBufferListStrategy& other) noexcept;
    friend void swap(GrowingBufferListStrategy& lhs,
                     GrowingBufferListStrategy& rhs)
    {
        lhs.swap(rhs);
    }

    ///allocate an initial buffer of \a initial_size bytes
    GrowingBufferListStrategy(std::size_t initial_size);
    ///set the initial buffer to an external \a initial_buffer of \a initial_size bytes
    GrowingBufferListStrategy(void* initial_buffer, 
                              std::size_t initial_size) noexcept;

    /// like the above constructors, but with a custom growth factor
    GrowingBufferListStrategy(float growth_factor) noexcept;
    GrowingBufferListStrategy(std::size_t initial_size, float growth_factor);
    GrowingBufferListStrategy(void* initial_buffer, std::size_t initial_size, 
                              float growth_factor) noexcept;

    ByteSpan getInitialBuffer() const noexcept
    {
        if (this->m_initial_buffer.ptr == nullptr)
          return ByteSpan{};
        return this->m_initial_buffer;
    }

    const std::vector<ByteSpan>& getAllocatedBuffers() const noexcept
    {
        return this->m_buffers;
    }

    void setGrowthFactor(float growth_factor)
    {
        if (growth_factor < 1.f) 
          throw std::invalid_argument("growth_factor must be >= 1.f");
        this->m_growth_factor = growth_factor;
    }
    float getGrowthFactor() const noexcept
    {
        return this->m_growth_factor;
    }

    /// release all allocated buffers
    void release() noexcept;


    /// align the pointer offset to the next multiple of \a alignment
    static std::size_t align(std::size_t offset, std::size_t alignment) noexcept; 

    /// add a new buffer to the list of buffers
    /// this new buffer shall be at least \a bytes bytes large 
    void pushBuffer(std::size_t bytes = 1);

    /// release the last allocated buffer
    void popBuffer();

  private:
    static ByteSpan newBuffer(std::size_t bytes);
    static void deleteBuffer(ByteSpan buffer) noexcept;
    std::size_t getCurrentBufferSize() const noexcept;

    ByteSpan m_initial_buffer = {nullptr, sizeof(Byte*)};
    std::vector<ByteSpan> m_buffers;
    float m_growth_factor = 2;
};



class LIFOMemoryResource ACHIBULUP__MEMORY_RESOURCE
{
  public:
    /// create an empty memory resouce that currently has no memory allocated
    /// and an implementation-defined growth factor
    LIFOMemoryResource() noexcept;
    LIFOMemoryResource(const LIFOMemoryResource&) = delete;
    /// move constructor allowed, but only in non-polymorphic context
    LIFOMemoryResource(LIFOMemoryResource&&) noexcept;
    void operator=(const LIFOMemoryResource&) = delete;
    /// move assignment allowed, but only in non-polymorphic context
    LIFOMemoryResource& operator=(LIFOMemoryResource&&) noexcept;
    virtual ~LIFOMemoryResource() = default;

    /// swapping two memory resources is allowed, but only in non-polymorphic context
    void swap(LIFOMemoryResource& other) noexcept;
    friend void swap(LIFOMemoryResource& lhs, LIFOMemoryResource& rhs)
    {
        lhs.swap(rhs);
    }

    ///allocate an initial buffer of \a initial_size bytes
    LIFOMemoryResource(std::size_t initial_size);
    ///set the initial buffer to an external \a initial_buffer of \a initial_size bytes
    LIFOMemoryResource(void* initial_buffer, std::size_t initial_size) noexcept;

    /// like the above constructors, but with a custom growth factor
    LIFOMemoryResource(float growth_factor) noexcept;
    LIFOMemoryResource(std::size_t initial_size, float growth_factor);
    LIFOMemoryResource(void* initial_buffer, std::size_t initial_size, 
                       float growth_factor) noexcept;

    ///thin wrappers are inlined
#if !ACHIBULUP__have_memory_resource
    void* allocate(std::size_t bytes, 
                   std::size_t alignment = alignof(std::max_align_t))
    {
        return this->do_allocate(bytes, alignment);
    }
    /// deallocate the last allocated memory
    /// the \a size and \a alignment parameters are ignored
    /// the behavior is undefined if \a p is neither nullptr nor the last allocated memory
    /// otherwise, the behaviour is the same as deallocateLast()
    /// this function is to conform to the std::pmr::memory_resource interface
    void deallocate(void* p, std::size_t bytes, 
                    std::size_t alignment = alignof(std::max_align_t)) noexcept
    {
        this->do_deallocate(p, bytes, alignment);
    }
#endif 
    /// same as the above deallocate
    void deallocate(void *p) noexcept
    {
        this->do_deallocate(p, 0, alignof(std::max_align_t));
    }
    void deallocateLast() noexcept;

    void setGrowthFactor(float growth_factor) noexcept
    {
        this->m_buffer_manager.setGrowthFactor(growth_factor);
    }
    float getGrowthFactor() const noexcept
    {
        return this->m_buffer_manager.getGrowthFactor();
    }

    /// release all unused buffers
    void shrinkToFit() noexcept;
    /// release all allocated buffers
    void release() noexcept;

  protected:
    ByteSpan getCurrentBuffer() noexcept;

    /// align the pointer offset to the next multiple of \a alignment
    static std::size_t align(std::size_t offset, std::size_t alignment) noexcept; 

  private:
    virtual void* do_allocate(std::size_t bytes, std::size_t alignment);
    /// if p is nullptr, this does nothing, otherwise calls deallocateLast()
    virtual void do_deallocate(void *p, std::size_t bytes, 
                               std::size_t alignment);
#if ACHIBULUP__have_memory_resource
    bool do_is_equal(const std::pmr::memory_resource&) const noexcept override;
#endif

    /// set the current buffer to the first next buffer in the list that have at least \a bytes bytes of \a alignment
    /// if no such buffer exists, call pushBuffer() to add a new buffer
    /// return the aligned offset from the begin of the current buffer
    std::size_t advanceBuffer(std::size_t bytes, std::size_t alignment);

  protected:
    /// m_current_buffer == -1 means that the current buffer is the initial (external) buffer
    std::ptrdiff_t m_current_buffer = -1;
    std::vector<std::size_t> m_allocation_offsets;
    GrowingBufferListStrategy m_buffer_manager;
};

inline bool operator == (const LIFOMemoryResource &lhs, 
                         const LIFOMemoryResource &rhs) noexcept
{
    return &lhs == &rhs;
}
inline bool operator != (const LIFOMemoryResource &lhs, 
                         const LIFOMemoryResource &rhs) noexcept
{
    return !(lhs == rhs);
}



template<std::size_t BUFFERSIZE>
class BufferedLIFOMemResource : public LIFOMemoryResource
{
    using Base = LIFOMemoryResource;
  public:
    BufferedLIFOMemResource() noexcept : Base(this->m_buffer, BUFFERSIZE) {}
    BufferedLIFOMemResource(const BufferedLIFOMemResource&) = delete;
    void operator=(const BufferedLIFOMemResource&) = delete;
    void swap(BufferedLIFOMemResource&) noexcept = delete;

    BufferedLIFOMemResource(float growth_factor) noexcept
    : BufferedLIFOMemResource()
    {
        this->setGrowthFactor(growth_factor);
    }

  protected:
    Byte m_buffer[BUFFERSIZE] alignas(alignof(std::max_align_t));
};

template<std::size_t BUFFERSIZE1, std::size_t BUFFERSIZE2>
void swap(BufferedLIFOMemResource<BUFFERSIZE1>&,
          BufferedLIFOMemResource<BUFFERSIZE2>&) = delete;

}

#endif // LIFOMemoryResource_HPP_INCLUDED