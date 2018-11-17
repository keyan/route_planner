#ifndef CONTRACTOR_H_
#define CONTRACTOR_H_

#include "graph_types.h"

class Contractor {
public:
  Contractor(RoadNetwork& road_network)
      : road_network_(road_network){};

  // Set node_ordering_ class member using a random shuffle.
  // Eventually sophisticated techniques can be used for efficient ordering.
  void compute_random_node_ordering();

  // Central contraction routine: contract the i-th node in the ordering,
  // ignoring nodes 1, ..., i - 1 in the ordering and their adjacent arcs.
  // IMPLEMENTATION NOTE: To ignore nodes (and their adjacent arcs), you can
  // simply use the arcFlag member of the Arc class. Initially, set all arcFlags
  // to 1, and as you go along and contract nodes, simply set the flags of the
  // arcs adjacent to contracted nodes to 0. And make sure to call
  // setConsiderArcFlags(true) on the dijkstra  object below.
  void contract_node(NodeID node_id);

  void dijkstra(NodeID& source, NodeID& middle_node, DistanceMap& distances,
      Weight max_weight, int max_nodes);

private:
  RoadNetwork& road_network_;

  // Dijkstras& dijkstras_;

  // Order to contract nodes, where len(vec) == total nodes n, and vec[i] is
  // some value between 0...n (without repeating).
  std::vector<NodeID> node_ordering_;
};

#endif // CONTRACTOR_H_
