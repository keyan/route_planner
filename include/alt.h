#include <vector>

#include "dijkstras.h"
#include "graph_types.h"
#include "road_network.h"

// An implementation of A* using landmark nodes and the triangle inequality as
// as a heuristic.
//
// By precomputing the distance from _each_ landmark l to every node u our A*
// heuristic is a lower bound represented as:
//
// (1)  dist(u, l) <= dist(u, v) + dist(v, l)
// (2)  dist(u, l) - dist(v, l) <= dist(u, v)
//
// Because our graph is undirected we can avoid the complexity of precomputing
// distance _to_ each landmark and instead take the absolute value of the left
// side of (2).
//
// See: https://dl.acm.org/citation.cfm?doid=1070432.1070455
// "Computing the shortest path: A search meets graph theory"
class ALT {
public:
  ALT(RoadNetwork& road_network, Dijkstras& dijkstras)
      : road_network_(road_network)
      , dijkstras_(dijkstras) {
    select_landmarks();
    precompute_landmark_map();
  }

  // Compute shortest path using A* with the landmark heuristic.
  Weight search(NodeID const& source_node_id, NodeID const& target_node_id);

  std::vector<NodeID> landmarks_;

  // Precomputation for distances to every node in the network from all
  // landmarks, index of each entry is the pointer to the NodeID in landmarks_.
  std::vector<std::vector<Weight>> landmark_distances_;

private:
  // Select a constant number of nodes for landmark computation.
  //
  // There are empirically better performing methods for selection than random
  // assignment, but use that for ease for now.
  void select_landmarks(int num_landmarks = 10);

  // Precompute landmark_distances_.
  void precompute_landmark_map();

  RoadNetwork& road_network_;
  Dijkstras& dijkstras_;
};
