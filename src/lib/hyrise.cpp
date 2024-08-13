#include "hyrise.hpp"

#include <memory>

#include <boost/container/pmr/global_resource.hpp>

#include "concurrency/transaction_manager.hpp"
#include "scheduler/abstract_scheduler.hpp"
#include "scheduler/immediate_execution_scheduler.hpp"
#include "scheduler/topology.hpp"
#include "storage/storage_manager.hpp"
#include "utils/log_manager.hpp"
#include "utils/meta_table_manager.hpp"
#include "utils/plugin_manager.hpp"
#include "utils/settings_manager.hpp"

namespace hyrise {

Hyrise::Hyrise() {
  // The default_memory_resource must be initialized before Hyrise's members so that it is destructed after them and
  // remains accessible during their deconstruction. For example, when the StorageManager is destructed, it causes its
  // stored tables to be deconstructed, too. As these might call deallocate on the default_memory_resource, it is
  // important that the resource has not been destructed before. As objects are destructed in the reverse order of their
  // construction, explicitly initializing the resource first means that it is destructed last.
  boost::container::pmr::get_default_resource();
#ifdef HYRISE_WITH_MOSES
  places = {
    {"base", moses::Place("/mnt/moses/base", "base", moses::contention::LOW)},
    {"table", moses::Place("/mnt/moses/long_lived", "table", moses::contention::LOW)},
    {"part", moses::Place("/mnt/moses/long_lived", "part", moses::contention::LOW)},
    {"nation", moses::Place("/mnt/moses/long_lived", "nation", moses::contention::LOW)},
    {"orders", moses::Place("/mnt/moses/long_lived", "orders", moses::contention::LOW)},
    {"region", moses::Place("/mnt/moses/long_lived", "region", moses::contention::LOW)},
    {"supplier", moses::Place("/mnt/moses/long_lived", "supplier", moses::contention::LOW)},
    {"lineitem", moses::Place("/mnt/moses/long_lived", "lineitem", moses::contention::LOW)},
    {"partsupp", moses::Place("/mnt/moses/long_lived", "partsupp", moses::contention::LOW)},
    {"customer", moses::Place("/mnt/moses/long_lived", "customer", moses::contention::LOW)},
    {"table_statistics", moses::Place("/mnt/moses/table_statistics", "table_statistics", moses::contention::LOW)},
    {"temp", moses::Place("/mnt/moses/short_lived", "temp", moses::contention::HIGH)},
    {"aggregate", moses::Place("/mnt/moses/short_lived", "aggregate", moses::contention::HIGH)},
    {"joinhash", moses::Place("/mnt/moses/short_lived", "joinhash", moses::contention::HIGH)},
    {"projection", moses::Place("/mnt/moses/short_lived", "projection", moses::contention::HIGH)},
    {"gettable", moses::Place("/mnt/moses/short_lived", "gettable", moses::contention::HIGH)},
    {"sort", moses::Place("/mnt/moses/short_lived", "sort", moses::contention::HIGH)},
    {"tablescan", moses::Place("/mnt/moses/short_lived", "tablescan", moses::contention::HIGH)},
    };
  moses::Moses::Initialize(&places);
  moses::PlaceGuard guard(&places.at("base"));
#endif

  storage_manager = StorageManager{};
  plugin_manager = PluginManager{};
  transaction_manager = TransactionManager{};
  meta_table_manager = MetaTableManager{};
  settings_manager = SettingsManager{};
  log_manager = LogManager{};
  topology = Topology{};
  _scheduler = std::make_shared<ImmediateExecutionScheduler>();
}

void Hyrise::reset() {
  Hyrise::get().scheduler()->finish();
  get() = Hyrise{};
}

const std::shared_ptr<AbstractScheduler>& Hyrise::scheduler() const {
  return _scheduler;
}

bool Hyrise::is_multi_threaded() const {
  return std::dynamic_pointer_cast<ImmediateExecutionScheduler>(_scheduler) == nullptr;
}

void Hyrise::set_scheduler(const std::shared_ptr<AbstractScheduler>& new_scheduler) {
  _scheduler->finish();
  _scheduler = new_scheduler;
  _scheduler->begin();
}

}  // namespace hyrise
