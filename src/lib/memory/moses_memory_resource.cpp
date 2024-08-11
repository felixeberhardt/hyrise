#include "moses_memory_resource.hpp"

#include <boost/container/pmr/memory_resource.hpp>

#include "hyrise.hpp"
#include "utils/assert.hpp"

namespace hyrise {

#ifdef HYRISE_WITH_MOSES
MosesMemoryResource::MosesMemoryResource(const std::shared_ptr<moses::Place> place, const std::string name) : _place(place) {
#else
MosesMemoryResource::MosesMemoryResource() {
#endif

#ifdef HYRISE_WITH_MOSES
    _alloc = std::make_shared<moses::MosesAllocator>(place, name);
    //TODO: Maybe we need this, but at this point leave it as we can directly invoke the page manager
    //_place->AddPageManager(_page_manager);
#endif
}

void MosesMemoryResource::reserve(std::size_t bytes) {
#ifdef HYRISE_WITH_MOSES
  _alloc->Reserve(bytes);
#endif
}

void* MosesMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
#ifdef HYRISE_WITH_MOSES
  void *addr = _alloc->Allocate(bytes);
  return addr;
#else
  return malloc(bytes);
#endif
}

void MosesMemoryResource::do_deallocate(void* pointer, std::size_t bytes, std::size_t alignment) {
#ifdef HYRISE_WITH_MOSES
  _alloc->Deallocate(pointer, bytes);
#else
  free(pointer);
#endif
}

bool MosesMemoryResource::do_is_equal(const memory_resource& other) const noexcept {
  return &other == this;
}

}  // namespace hyrise