#include <algorithm>

#include "contractor.h"

void Contractor::compute_random_node_ordering() {
  if (!node_ordering_.empty()) { return; }

  for (Node node : road_network_.graph_) {
    node_ordering_.emplace_back(node.id_);
  }
  std::random_shuffle(node_ordering_.begin(), node_ordering_.end());
}

void Contractor::contract_node(NodeID node_id) {
  Node const& node = road_network_.graph_.at(node_id);

  for (auto i = node.outgoing_edges_.begin(); i != node.outgoing_edges_.end();
      ++i) {
    if ((*i).downwards_) { continue; }
    for (auto j = std::next(i); j != node.outgoing_edges_.end(); ++j) {
      if ((*j).downwards_) { continue; }
      Weight curr_distance = (*i).weight_ + (*j).weight_;
      dijkstra(
    }
  }
}

void Contractor::dijkstra(NodeID& source, NodeID& middle_node,
    DistanceMap& distances, Weight max_weight, int max_nodes) {
  distances[node] = 0;

  MinHeapPriorityQueue node_queue;
  node_queue.emplace(WeightedNode(source_node, 0));

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
}


