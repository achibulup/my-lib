#include "LIFOMemoryResource.hpp"

namespace Achibulup
{

GrowingBufferListStrategy::GrowingBufferListStrategy() noexcept {}
GrowingBufferListStrategy::
GrowingBufferListStrategy(GrowingBufferListStrategy &&other) noexcept
: GrowingBufferListStrategy()
{
    this->swap(other);
}
GrowingBufferListStrategy& GrowingBufferListStrategy::
operator = (GrowingBufferListStrategy &&other) noexcept
{
    if (this != &other)
      this->swap(other);    
    return *this;
}
GrowingBufferListStrategy::~GrowingBufferListStrategy()
{
    this->release();
}

void GrowingBufferListStrategy::swap(GrowingBufferListStrategy &other) noexcept
{
    using std::swap;
    swap(this->m_initial_buffer, other.m_initial_buffer);
    swap(this->m_buffers, other.m_buffers);
    swap(this->m_growth_factor, other.m_growth_factor);
}

GrowingBufferListStrategy::
GrowingBufferListStrategy(std::size_t initial_size)
: m_initial_buffer(nullptr, initial_size)
{
    this->m_buffers.push_back(newBuffer(initial_size));
}

GrowingBufferListStrategy::
GrowingBufferListStrategy(void* initial_buffer, 
                          std::size_t initial_size) noexcept
: m_initial_buffer(initial_buffer, initial_size) {}

GrowingBufferListStrategy::
GrowingBufferListStrategy(float growth_factor) noexcept
: GrowingBufferListStrategy()
{
    this->setGrowthFactor(growth_factor);
}

GrowingBufferListStrategy::
GrowingBufferListStrategy(std::size_t initial_size, float growth_factor)
: GrowingBufferListStrategy(initial_size)
{
    this->setGrowthFactor(growth_factor);
}

GrowingBufferListStrategy::
GrowingBufferListStrategy(void* initial_buffer, std::size_t initial_size, 
                          float growth_factor) noexcept
: GrowingBufferListStrategy(initial_buffer, initial_size)
{
    this->setGrowthFactor(growth_factor);
}


void GrowingBufferListStrategy::release() noexcept
{
    while (!this->m_buffers.empty())
      this->popBuffer();
}

void GrowingBufferListStrategy::popBuffer()
{
    if (this->m_buffers.empty()) 
      throw std::runtime_error("GrowingBufferListStrategy::popBuffer: "
                               "no buffer to pop");
    deleteBuffer(this->m_buffers.back());
    this->m_buffers.pop_back();
}

void GrowingBufferListStrategy::pushBuffer(std::size_t bytes)
{
    std::size_t next_buffer_size;
    if (this->m_buffers.empty())
      next_buffer_size = std::max<size_t>(this->getCurrentBufferSize(), bytes);
    else 
      next_buffer_size = std::max<size_t>(
          this->getCurrentBufferSize() * this->m_growth_factor, bytes);
    this->m_buffers.push_back(newBuffer(next_buffer_size));
}

std::size_t GrowingBufferListStrategy::getCurrentBufferSize() const noexcept
{
    return this->m_buffers.empty() ? this->m_initial_buffer.size :
                                     this->m_buffers.back().size;
}

ByteSpan GrowingBufferListStrategy::newBuffer(std::size_t bytes)
{
    return ByteSpan(bytes != 0 ? ::operator new(bytes) : nullptr, bytes);
}

void GrowingBufferListStrategy::deleteBuffer(ByteSpan buffer) noexcept
{
    ::operator delete(buffer.ptr);
}



LIFOMemoryResource::LIFOMemoryResource() noexcept : m_buffer_manager() {}
LIFOMemoryResource::LIFOMemoryResource(LIFOMemoryResource &&other) noexcept
: LIFOMemoryResource()
{
    this->swap(other);
}
LIFOMemoryResource& LIFOMemoryResource::
operator = (LIFOMemoryResource &&other) noexcept
{
    if (this != &other) {
      this->swap(other);
      other.m_allocation_offsets.clear();
      other.m_current_buffer = -1;
    }
    return *this;
}

void LIFOMemoryResource::swap(LIFOMemoryResource &other) noexcept
{
    using std::swap;
    swap(this->m_buffer_manager, other.m_buffer_manager);
    swap(this->m_allocation_offsets, other.m_allocation_offsets);
    swap(this->m_current_buffer, other.m_current_buffer);
}

LIFOMemoryResource::
LIFOMemoryResource(std::size_t initial_size)
: m_buffer_manager(initial_size) {}

LIFOMemoryResource::
LIFOMemoryResource(void* initial_buffer, 
                   std::size_t initial_size) noexcept
: m_buffer_manager(initial_buffer, initial_size) {}

LIFOMemoryResource::
LIFOMemoryResource(float growth_factor) noexcept
: LIFOMemoryResource()
{
    this->setGrowthFactor(growth_factor);
}

LIFOMemoryResource::
LIFOMemoryResource(std::size_t initial_size, float growth_factor)
: LIFOMemoryResource(initial_size)
{
    this->setGrowthFactor(growth_factor);
}

LIFOMemoryResource::
LIFOMemoryResource(void* initial_buffer, std::size_t initial_size, 
                          float growth_factor) noexcept
: LIFOMemoryResource(initial_buffer, initial_size)
{
    this->setGrowthFactor(growth_factor);
}

void LIFOMemoryResource::deallocateLast() noexcept
{
    this->m_allocation_offsets.pop_back();
    while (!this->m_allocation_offsets.empty() 
        && this->m_allocation_offsets.back() == 0) {
      --this->m_current_buffer;
      this->m_allocation_offsets.pop_back();
    }
}

void LIFOMemoryResource::shrinkToFit() noexcept
{
    std::size_t pop_count = this->m_buffer_manager.getAllocatedBuffers().size() 
                            - this->m_current_buffer - 1;
    while (pop_count --> 0)
      this->m_buffer_manager.popBuffer();
}

void LIFOMemoryResource::release() noexcept
{
    this->m_current_buffer = -1;
    this->m_allocation_offsets.clear();
    this->m_buffer_manager.release();
}

ByteSpan LIFOMemoryResource::getCurrentBuffer() noexcept
{
    if (this->m_current_buffer == -1) 
      return this->m_buffer_manager.getInitialBuffer();
    return this->m_buffer_manager.getAllocatedBuffers()[this->m_current_buffer];
}

std::size_t LIFOMemoryResource::
align(std::size_t bytes, std::size_t alignment) noexcept
{
    return (bytes + alignment - 1) & ~(alignment - 1);
}

void* LIFOMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
{
    if (bytes == 0) return do_allocate(1, alignment);

    std::size_t offset = this->advanceBuffer(bytes, alignment);
    return this->getCurrentBuffer().ptr + offset;
}

void LIFOMemoryResource::do_deallocate(void *p, std::size_t, std::size_t)
{
    if (p == nullptr) return;
    this->deallocateLast();
}

#if ACHIBULUP__have_memory_resource
bool LIFOMemoryResource::
do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}
#endif


std::size_t LIFOMemoryResource::
advanceBuffer(std::size_t bytes, std::size_t alignment)
{
    /// the offset if we use the curent buffer
    std::size_t last_allocation_offset = this->m_allocation_offsets.empty() ? 
        0 : this->m_allocation_offsets.back();
    std::size_t offset = align(last_allocation_offset, alignment);
    if (offset + bytes <= this->getCurrentBuffer().size) {
      this->m_allocation_offsets.push_back(offset + bytes);
      return offset;
    }

    offset = 0;
    /// implement strong exception safety
    std::size_t current_buffer_backup = this->m_current_buffer;
    std::size_t alloc_log_backup = this->m_allocation_offsets.size();

    try {
      std::size_t n_buffers 
          = this->m_buffer_manager.getAllocatedBuffers().size();
      do {
        ++this->m_current_buffer;
        this->m_allocation_offsets.push_back(0);
        if (this->m_current_buffer == n_buffers)
          break;
      } while (bytes > this->getCurrentBuffer().size);
      if (this->m_current_buffer == n_buffers)
        this->m_buffer_manager.pushBuffer(bytes);
      this->m_allocation_offsets.push_back(offset + bytes);
    }
    catch (...) {
      this->m_current_buffer = current_buffer_backup;
      while (this->m_allocation_offsets.size() > alloc_log_backup)
        this->m_allocation_offsets.pop_back();
      throw;
    }

    return offset;
}


} // namespace Achibulup

    