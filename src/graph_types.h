#ifndef GRAPH_TYPES_H_
#define GRAPH_TYPES_H_
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>

using NodeID = int64_t;
using EdgeID = int64_t;
using Weight = int64_t;

using NodeIDList = std::vector<NodeID>;
using NodeIDSet = std::unordered_set<NodeID>;

constexpr Weight INF_WEIGHT = std::numeric_limits<Weight>::max();

// Represents a directed edge between two nodes.
//
// These are constructed from OSM ways.
struct Edge {
  Edge(NodeID head_node_id, Weight weight)
      : head_node_id_(head_node_id)
      , weight_(weight) {}

  NodeID head_node_id_;
  // Travel time in seconds.
  Weight weight_;
};

// Represents a single vertex in the graph, maps directly to an OSM node.
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

// Convenience object for representing a Node and Weight together.
struct WeightedNode {
  WeightedNode(NodeID node_id, Weight weight)
      : node_id_(node_id)
      , weight_(weight) {}

  NodeID node_id_;
  Weight weight_;

  bool operator<(const WeightedNode& other) const {
    return weight_ < other.weight_;
  }

  bool operator>(const WeightedNode& other) const {
    return weight_ > other.weight_;
  }
};

#endif // GRAPH_TYPES_H_
