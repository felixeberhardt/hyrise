#include "immediate_execution_scheduler.hpp"
#include <numaif.h>
#include <numa.h>

namespace opossum {
ImmediateExecutionScheduler::ImmediateExecutionScheduler(int node) {
  if(node != -1) {
    struct bitmask *pol_mask;
    int nnodes = numa_max_possible_node() + 1;
    pol_mask = numa_bitmask_alloc(nnodes);
    numa_bitmask_setbit(pol_mask, node);
    long ret = set_mempolicy(MPOL_BIND, pol_mask->maskp, pol_mask->size + 1);
    if (ret == -1)
    {
      fprintf(stderr, "error: %s\n", strerror(errno));
    } 
  }
}

void ImmediateExecutionScheduler::begin() {}

void ImmediateExecutionScheduler::wait_for_all_tasks() {}

void ImmediateExecutionScheduler::finish() {}

bool ImmediateExecutionScheduler::active() const {
  return false;
}

const std::vector<std::shared_ptr<TaskQueue>>& ImmediateExecutionScheduler::queues() const {
  return _queues;
}

void ImmediateExecutionScheduler::schedule(std::shared_ptr<AbstractTask> task, NodeID preferred_node_id,
                                           SchedulePriority priority) {
  DebugAssert(task->is_scheduled(), "Don't call ImmediateExecutionScheduler::schedule(), call schedule() on the task");

  if (task->is_ready()) {
    task->execute();
  } else {
    // If a task is not yet ready, its predecessors must be executed first.
    for (const auto& predecessor_task : task->predecessors()) {
      predecessor_task.lock()->schedule();
    }
  }

  Assert(task->is_done(), "Task should have been executed by now.");
}

}  // namespace opossum
