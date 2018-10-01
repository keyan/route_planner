#ifndef GRAPH_TYPES_H_
#define GRAPH_TYPES_H_
#include <cstdint>
#include <vector>

// Represents a directed edge between two nodes.
struct Edge {
  Edge(int64_t head_node_id, int weight)
      : head_node_osm_id_(head_node_id)
      , weight_(weight) {}

  int64_t head_node_osm_id_;
  // Travel time in seconds.
  int weight_;
};

struct Node {
  Node(int64_t osm_id, double lat, double lng)
      : osm_id_(osm_id)
      , lat_(lat)
      , lng_(lng) {}

  int osm_id_;
  double lat_;
  double lng_;
  std::vector<Edge> outgoing_edges_;
};

struct WeightedNode {
  WeightedNode(Node& node, int weight)
      : node_(node)
      , weight_(weight) {}

  Node& node_;
  int weight_;
};

struct CompareWeightedNodes {
  bool operator()(WeightedNode& n1, WeightedNode& n2) {
    return n1.weight_ < n2.weight_;
  }
};

#endif // GRAPH_TYPES_H_
