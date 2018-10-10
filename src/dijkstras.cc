#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>

#include "dijkstras.h"
#include "road_network.h"

int Dijkstras::search(int64_t source_node_id, int64_t target_node_id) {
  // Track how to reach each node in path, node_id -> weight
  std::unordered_map<int64_t, int> parents;

  // pq = std::priority_queue<WeightedNode, std::vector<WeightedNode>,
  // CompareWeightedNodes>; pq.push();

  return 0;
}
