#include "benchmark_item_run_result.hpp"

namespace opossum {

BenchmarkItemRunResult::BenchmarkItemRunResult(Duration init_begin, Duration init_duration,
                                                std::unordered_map<std::string, double> init_perf_counter,
                                                std::vector<SQLPipelineMetrics> init_metrics)
    : begin(init_begin), duration(init_duration), perf_counter(std::move(init_perf_counter)), metrics(std::move(init_metrics)) {}

}  // namespace opossum
