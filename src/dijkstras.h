#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include "road_network.h"

class Dijkstras {
public:
  Dijkstras(RoadNetwork& graph)
      : graph_(graph) {
    // Initialize connected component map
    for (Node* node : graph_.nodes_) {
      visited_nodes_[node->osm_id_] = -1;
    }
  }

  // Compute the shortest path from the single source to single destination.
  //
  // Returns the weight of the shortest path. If called with target node -1,
  // Dijkstra is run until all nodes reachable from the source are settled, in
  // this case the return value is only an exit code.
  int search(int64_t source_node_id, int64_t target_node_id);

  // Set round for connected components determination
  void set_round(int mark);

private:
  RoadNetwork& graph_;
  // Used to delineate connected components
  int round_;

  // Indicator which node was visited by a particular run of Dijkstras. Useful
  // for computing the connected components.
  std::unordered_map<int64_t, int> visited_nodes_;
};
