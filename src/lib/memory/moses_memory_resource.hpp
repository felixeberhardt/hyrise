#pragma once

#include <boost/container/pmr/memory_resource.hpp>

#include "types.hpp"

#ifdef HYRISE_WITH_MOSES
#include "moses_allocator.h"
#else

#endif

namespace hyrise {

/**
 * The base memory resource for MOSES memory allocation.
 *
 * Each MosesMemoryResource is intended to correspond to exactly one MOSES place.
 */
class MosesMemoryResource : public boost::container::pmr::memory_resource {
 public:
  // Constructor creating an arena for a specific node.
#ifdef HYRISE_WITH_MOSES
  explicit MosesMemoryResource(const std::shared_ptr<moses::Place> place, const std::string name);
#else
  explicit MosesMemoryResource();
#endif
  void reserve(std::size_t bytes);
  // Methods defined by memory_resource.
  void* do_allocate(std::size_t bytes, std::size_t alignment) override;
  /**
   * Entry point for deallocation behavior.
   */
  void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;
  bool do_is_equal(const memory_resource& other) const noexcept override;

 private:
#ifdef HYRISE_WITH_MOSES
  std::shared_ptr<moses::Place> _place;
  std::shared_ptr<moses::MosesAllocator> _alloc;
#endif
};

}  // namespace hyrise