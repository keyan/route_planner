#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include "graph_types.h"
#include "road_network.h"

class Dijkstras {
public:
  Dijkstras(RoadNetwork& graph)
      : graph_(graph)
      , visited_nodes_(graph.graph_.size(), -1) {
    ;
  }

  // Compute the shortest path from the single source to single destination.
  //
  // Returns the weight of the shortest path. If called with:
  //  - target node -1:
  //    Will run until all nodes reachable from the source are settled, in
  //    this case the return value is only an exit code.
  //  - heuristic_ is non-null:
  //    heuristic[NodeID] is added to weights before nodes are added in/back
  //    the the priority queue.
  Weight search(
      NodeID const& source_node_id,
      NodeID const& target_node_id,
      DistanceMap& distances);

  // Set round for connected components determination
  void set_round(int64_t mark);

  // Set a vector mapping from NodeID (index) -> heuristic weight, to append
  // to the Node weight during A* searches.
  void set_heuristic(std::vector<Weight>* heuristic);

  // Indicator which node was visited by a particular run of Dijkstras. Useful
  // for computing the connected components.
  std::vector<int64_t> visited_nodes_;

private:
  Weight get_heuristic_weight(NodeID const& node_id);

  RoadNetwork& graph_;

  // Used to delineate connected components
  int64_t round_ = 0;

  // Mapping of NodeID -> max distance to a landmark.
  std::vector<Weight>* heuristic_ = nullptr;
};
