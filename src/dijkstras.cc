#include <utility>

#include "dijkstras.h"

Weight Dijkstras::search(
    NodeID const& source_node_id,
    NodeID const& target_node_id,
    DistanceMap& distances) {
  shortest_path_tree_.clear();
  NodeMap parents = {{source_node_id, -1}};
  distances[source_node_id] = 0;

  MinHeapPriorityQueue node_queue;
  node_queue.emplace(WeightedNode(source_node_id, 0));

  while (!node_queue.empty()) {
    WeightedNode curr_w_node = node_queue.top();
    node_queue.pop();

    NodeID const& curr_node_id = curr_w_node.node_id_;
    Node const& curr_node = graph_.graph_[curr_node_id];

    // std::priority_queue has no decrease-weight operation, instead do a "lazy
    // deletion" by keeping the old node in the pq and just ignoring it when it
    // is eventually popped.
    if (shortest_path_tree_.count(curr_node_id) == 1) {
      continue;
    }

    // Add removed node to shortest path tree.
    shortest_path_tree_[curr_node_id] = parents[curr_node_id];

    if (curr_node_id == target_node_id) {
      break;
    }

    // Mark round visted for connected component determination.
    visited_nodes_[curr_node_id] = round_;

    for (Edge const& adj_edge : curr_node.outgoing_edges_) {
      NodeID const& adj_node_id = adj_edge.head_node_id_;

      if (shortest_path_tree_.count(adj_node_id) == 0) {
        Weight const& curr_weight =
            distances.insert(std::make_pair(adj_node_id, INF_WEIGHT))
                .first->second;
        Weight new_weight = distances[curr_node_id] + adj_edge.weight_;
        // Relax edge
        if (curr_weight > new_weight) {
          distances[adj_node_id] = new_weight;
          parents[adj_node_id] = curr_node_id;

          node_queue.emplace(WeightedNode(
              adj_node_id, new_weight + get_heuristic_weight(adj_node_id)));
        }
      }
    }
  }

  if (distances.count(target_node_id) == 0 or target_node_id == -1) {
    return INF_WEIGHT;
  } else {
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
