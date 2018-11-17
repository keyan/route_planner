#ifndef GRAPH_TYPES_H_
#define GRAPH_TYPES_H_

#include <cstdint>
#include <functional>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using NodeID = int32_t;
using OSMNodeID = int64_t;
using EdgeID = int32_t;
using Weight = int32_t;

using NodeIDList = std::vector<NodeID>;
using NodeIDSet = std::unordered_set<NodeID>;

using DistanceMap = std::unordered_map<NodeID, Weight>;
using NodeMap = std::unordered_map<NodeID, NodeID>;

constexpr Weight INF_WEIGHT = std::numeric_limits<Weight>::max();

struct LatLng {
  LatLng(float lat, float lng)
      : lat_(lat)
      , lng_(lng) {}

  float lat_;
  float lng_;
};

// Represents a directed edge between two nodes.
//
// These are constructed from OSM ways.
struct Edge {
  Edge(NodeID head_node_id, Weight weight)
      : head_node_id_(head_node_id)
      , weight_(weight) {}

  Edge(const Edge& that) {
    head_node_id_ = that.head_node_id_;
    weight_ = that.weight_;
  }

  NodeID head_node_id_;
  // Travel time in seconds.
  Weight weight_;
  // When non-zero this value indicates that this edge was added as shortcut to
  // bypass the contracted_node_id_. Should be used for recursive contracted
  // graph unpacking.
  NodeID contracted_node_id_ = 0;
  // Whether the head node is in the downwards graph.
  //
  // Can we be used for indicating lower priority during bidirectional
  // hierarchical search or node contraction.
  bool downwards_ = false;
};

// Represents a single vertex in the graph, maps directly to an OSM node.
struct Node {
  Node(NodeID id, OSMNodeID osm_id, double lat, double lng)
      : id_(id)
      , osm_id_(osm_id)
      , lat_(lat)
      , lng_(lng) {}

  Node(const Node& that) {
    id_ = that.id_;
    osm_id_ = that.osm_id_;
    lat_ = that.lat_;
    lng_ = that.lng_;

    for (Edge const& edge : that.outgoing_edges_) {
      Edge new_edge = edge;
      outgoing_edges_.push_back(new_edge);
    }
  }

  NodeID id_;
  OSMNodeID osm_id_;
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

using MinHeapPriorityQueue = std::priority_queue<
    WeightedNode,
    std::vector<WeightedNode>,
    std::greater<std::vector<WeightedNode>::value_type>>;

#endif // GRAPH_TYPES_H_
