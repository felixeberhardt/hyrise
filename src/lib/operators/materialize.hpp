#pragma once

#include <memory>
#include <string>
#include <vector>

#include "abstract_read_only_operator.hpp"
#include "types.hpp"
#include "utils/assert.hpp"

namespace opossum {

/**
 * Materialize records of a table
 * within the context of a given query execution
 */
class Materialize : public AbstractReadOnlyOperator {
  friend class OperatorsValidateTest;

 public:
  explicit Materialize(const std::shared_ptr<AbstractOperator>& in);

  const std::string& name() const override;

 private:

 protected:
  std::shared_ptr<const Table> _on_execute(std::shared_ptr<TransactionContext> transaction_context) override;
  std::shared_ptr<const Table> _on_execute() override;
  std::shared_ptr<AbstractOperator> _on_deep_copy(
      const std::shared_ptr<AbstractOperator>& copied_left_input,
      const std::shared_ptr<AbstractOperator>& copied_right_input,
      std::unordered_map<const AbstractOperator*, std::shared_ptr<AbstractOperator>>& copied_ops) const override;
  void _on_set_parameters(const std::unordered_map<ParameterID, AllTypeVariant>& parameters) override;
};

}  // namespace opossum
