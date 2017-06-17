#pragma once

#include <bitset>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "abstract_join_operator.hpp"
#include "storage/dictionary_column.hpp"
#include "storage/reference_column.hpp"
#include "storage/value_column.hpp"
#include "type_comparison.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

/*
There are two nested loop joins, implemented by two groups: JoinNestedLoopA and B. They should be functionally
identical.

This operator joins two tables using one column of each table.
The output is a new table with referenced columns for all columns of the two inputs and filtered pos_lists.
If you want to filter by multiple criteria, you can chain this operator.

As with most operators, we do not guarantee a stable operation with regards to positions -
i.e., your sorting order might be disturbed.
*/

class JoinNestedLoopA : public AbstractJoinOperator {
 public:
  JoinNestedLoopA(const std::shared_ptr<const AbstractOperator> left,
                  const std::shared_ptr<const AbstractOperator> right,
                  optional<std::pair<std::string, std::string>> column_names, const std::string &op,
                  const JoinMode mode, const std::string &prefix_left = "", const std::string &prefix_right = "");

  std::shared_ptr<const Table> on_execute() override;

  const std::string name() const override;
  uint8_t num_in_tables() const override;
  uint8_t num_out_tables() const override;

 protected:
  std::unique_ptr<AbstractReadOnlyOperatorImpl> _impl;

  template <typename LeftType, typename RightType>
  class JoinNestedLoopAImpl;
};

}  // namespace opossum