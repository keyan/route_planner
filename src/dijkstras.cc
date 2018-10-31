#include <functional>
#include <queue>
#include <utility>

#include "binary_min_heap.h"
#include "dijkstras.h"
#include "graph_types.h"
#include "road_network.h"

using NodeMap = std::unordered_map<NodeID, NodeID>;
using MinHeapPriorityQueue = std::priority_queue<
    WeightedNode,
    std::vector<WeightedNode>,
    std::greater<std::vector<WeightedNode>::value_type>>;

Weight Dijkstras::search(
    NodeID const& source_node_id,
    NodeID const& target_node_id,
    DistanceMap& distances) {
  NodeMap parents = {{source_node_id, -1}};
  NodeMap shortest_path_tree;
  distances[source_node_id] = 0;

  const int NULL_DATA = 0;
  BinaryMinHeap<
      NodeID, NodeID, Weight, int, UnorderedMapStorage<NodeID, Weight>>
      node_queue(graph_.graph_.size());
  node_queue.Insert(source_node_id, 0, NULL_DATA);

  while (!node_queue.Empty()) {
    NodeID curr_node_id = node_queue.Min();
    node_queue.DeleteMin();

    Node const& curr_node = graph_.graph_[curr_node_id];

    // std::priority_queue has no decrease-weight operation, instead do a "lazy
    // deletion" by keeping the old node in the pq and just ignoring it when it
    // is eventually popped.
    if (shortest_path_tree.count(curr_node_id) == 1) {
      continue;
    }

    // Add removed node to shortest path tree.
    shortest_path_tree[curr_node_id] = parents[curr_node_id];

    if (curr_node_id == target_node_id) {
      break;
    }

    // Mark round visted for connected component determination.
    visited_nodes_[curr_node_id] = round_;

    for (Edge const& adj_edge : curr_node.outgoing_edges_) {
      NodeID const& adj_node_id = adj_edge.head_node_id_;

      if (shortest_path_tree.count(adj_node_id) == 0) {
        Weight const& curr_weight =
            distances.insert(std::make_pair(adj_node_id, INF_WEIGHT))
                .first->second;
        Weight new_weight = distances[curr_node_id] + adj_edge.weight_;
        // Relax edge
        if (curr_weight > new_weight) {
          distances[adj_node_id] = new_weight;
          parents[adj_node_id] = curr_node_id;

          new_weight += get_heuristic_weight(adj_node_id);
          if (!node_queue.WasInserted(adj_node_id)) {
            node_queue.Insert(adj_node_id, new_weight, NULL_DATA);
          } else {
            node_queue.DecreaseKey(adj_node_id, new_weight);
          }
        }
      }
    }
  }

  if (target_node_id == -1) {
    return 0;
  } else if (distances.count(target_node_id) == 0) {
    return INF_WEIGHT;
  } else {
    std::cout << "Settled nodes: " << shortest_path_tree.size() << std::endl;
    return distances[target_node_id];
  }
}

void Dijkstras::set_round(int64_t mark) { round_ = mark; }

void Dijkstras::set_heuristic(std::vector<Weight>* heuristic) {
  heuristic_ = heuristic;
};

Weight Dijkstras::get_heuristic_weight(NodeID const& node_id) {
  if (!heuristic_ or heuristic_->empty()) {
    return 0;
  } else {
    return heuristic_->at(node_id);
  }
}
