#include <csignal>
#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

#include "dijkstras.h"
#include "graph_types.h"
#include "road_network.h"

using DistanceMap = std::unordered_map<NodeID, Weight>;
using NodeMap = std::unordered_map<NodeID, NodeID>;

Weight
Dijkstras::search(NodeID const& source_node_id, NodeID const& target_node_id) {
  NodeMap parents = {{source_node_id, -1}};
  NodeMap shortest_path_tree;
  // Store mapping of minimal distance to each visited node.
  DistanceMap distances = {{source_node_id, 0}};

  std::priority_queue<
      WeightedNode, std::vector<WeightedNode>,
      std::less<std::vector<WeightedNode>::value_type>>
      node_queue;

  node_queue.emplace(WeightedNode(source_node_id, 0));

  while (!node_queue.empty()) {
    WeightedNode curr_w_node = node_queue.top();
    node_queue.pop();

    NodeID const& curr_node_id = curr_w_node.node_id_;
    Node const& curr_node = *(graph_.graph_[curr_node_id]);

    // Add removed node to shortest path tree.
    shortest_path_tree[curr_node_id] = parents[curr_node_id];
    // Mark round visted for connected component determination.
    visited_nodes_[curr_node_id] = round_;

    if (curr_node_id == target_node_id) {
      break;
    }

    for (Edge const& adj_edge : curr_node.outgoing_edges_) {
      NodeID const& adj_node_id = adj_edge.head_node_id_;

      if (shortest_path_tree.count(adj_node_id) == 0) {
        // Relax edge
        Weight curr_weight =
            distances.insert(std::make_pair(adj_node_id, INF_WEIGHT))
                .first->second;
        if (curr_weight > (distances[curr_node_id] + adj_edge.weight_)) {
          distances[adj_node_id] = distances[curr_node_id] + adj_edge.weight_;
          parents[adj_node_id] = curr_node_id;
        }

        node_queue.emplace(WeightedNode(adj_node_id, distances[adj_node_id]));
      }
    }
  }

  return distances[target_node_id];
}

void Dijkstras::set_round(int64_t mark) { round_ = mark; }
