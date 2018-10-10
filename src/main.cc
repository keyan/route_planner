#include <iostream>

#include "road_network.h"

int main() {
  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file("data/burlington.osm");
  road_network.reduce_to_largest_connected_component();

  std::cout << "Number of nodes: " << road_network.num_nodes_ << std::endl;
  std::cout << "Number of edges: " << road_network.num_edges_ << std::endl;

  return 0;
}
