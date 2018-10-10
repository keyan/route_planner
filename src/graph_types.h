#ifndef GRAPH_TYPES_H_
#define GRAPH_TYPES_H_
#include <cstdint>
#include <unordered_set>
#include <vector>

using NodeID = int64_t;
using EdgeID = int64_t;
using Weight = int;

using NodeIDList = std::vector<NodeID>;
using NodeIDSet = std::unordered_set<NodeID>;

// Represents a directed edge between two nodes.
struct Edge {
  Edge(NodeID head_node_id, int weight)
      : head_node_osm_id_(head_node_id)
      , weight_(weight) {}

  NodeID head_node_osm_id_;
  // Travel time in seconds.
  Weight weight_;
};

struct Node {
  Node(NodeID osm_id, double lat, double lng)
      : osm_id_(osm_id)
      , lat_(lat)
      , lng_(lng) {}

  NodeID osm_id_;
  double lat_;
  double lng_;
  std::vector<Edge> outgoing_edges_;
};

struct WeightedNode {
  WeightedNode(Node& node, Weight weight)
      : node_(node)
      , weight_(weight) {}

  Node& node_;
  Weight weight_;
};

struct CompareWeightedNodes {
  bool operator()(WeightedNode& n1, WeightedNode& n2) {
    return n1.weight_ < n2.weight_;
  }
};

#endif // GRAPH_TYPES_H_
