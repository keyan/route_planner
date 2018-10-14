#include <cstdlib>
#include <ctime>
#include <iostream>

#include "constants.h"
#include "dijkstras.h"
#include "graph_types.h"
#include "road_network.h"

int main() {
  // Seed random generator
  std::srand(std::time(0));

  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file("data/burlington.osm");
  std::cout << "Number of nodes: " << road_network.num_nodes_ << std::endl;
  std::cout << "Number of edges: " << road_network.num_edges_ << std::endl;

  std::cout << "Reducing graph..." << std::endl;
  road_network.reduce_to_largest_connected_component();

  std::cout << "LCC # of nodes: " << road_network.num_nodes_ << std::endl;
  std::cout << "LCC # of edges: " << road_network.num_edges_ << std::endl;

  Dijkstras dijkstra = Dijkstras(road_network);
  for (int i = 0; i < 5; i++) {
    Node const& origin = road_network.get_rand_node();
    Node const& target = road_network.get_rand_node();

    Weight result = dijkstra.search(origin.osm_id_, target.osm_id_);
    std::cout << "https://www.google.com/maps/dir/" << origin.lat_ << ","
              << origin.lng_ << "/" << target.lat_ << "," << target.lng_
              << " time: " << (result / MS_IN_MINUTE) << "m"
              << (result / MS_IN_MINUTE) % 60 << "s" << std::endl;
  }

  return 0;
}
