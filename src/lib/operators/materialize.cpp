#include "materialize.hpp"

#include <vector>

namespace opossum {

Materialize::Materialize(const std::shared_ptr<AbstractOperator>& in)
    : AbstractReadOnlyOperator(OperatorType::Materialize, in) {}

const std::string& Validate::name() const {
  static const auto name = std::string{"Materialize"};
  return name;
}

std::shared_ptr<AbstractOperator> Materialize::_on_deep_copy(
    const std::shared_ptr<AbstractOperator>& copied_left_input,
    const std::shared_ptr<AbstractOperator>& copied_right_input,
    std::unordered_map<const AbstractOperator*, std::shared_ptr<AbstractOperator>>& copied_ops) const {
  return std::make_shared<Materialize>(copied_left_input);
}

void Materialize::_on_set_parameters(const std::unordered_map<ParameterID, AllTypeVariant>& parameters) {}

std::shared_ptr<const Table> Materialize::_on_execute() {
  const auto in_table = left_input_table();
  const auto chunk_count = in_table->chunk_count();
  auto out_chunks = std::vector<std::shared_ptr<Chunk>>(chunk_count);

  for (ChunkID in_chunk_id{0u}; in_chunk_id < chunk_count; in_chunk_id++) {
    const auto chunk = in_table->get_chunk(in_chunk_id);
    Segments segments{column_count()};
    for (ColumnID column_id{0u}; column_id < input_column_count(); ++column_id) { 
      const auto data_type = in_table->column_data_type(column_id);
      std::shared_ptr<AbstractSegment> cur_seg = chunk->get_segment(column_id);
      resolve_data_type(data_type, [&](auto type) {
            using ColumnDataType = typename decltype(type)::type;
            auto output = std::vector<ColumnDataType>{};
            output.reserve(cur_seg->size());
	    materialize_values_and_nulls(cur_seg, output);
	    segments.append(output);
      });
    }
    out_chunks.append(std::make_shared<Chunk>(segments));
  }

  return std::make_shared<Table>(in_table->column_definitions(), TableType::Data, std::move(out_chunks));
}
}  // namespace opossum
