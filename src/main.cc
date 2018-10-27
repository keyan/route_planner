#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "constants.h"
#include "dijkstras.h"
#include "graph_types.h"
#include "road_network.h"
#include "utils.h"

int main() {
  // Seed random generator
  std::srand(std::time(0));

  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file("data/burlington.osm");
  // road_network.load_from_osm_file("data/saarland.osm");
  std::cout << "Number of nodes: " << road_network.graph_.size() << std::endl;
  std::cout << "Number of edges: " << road_network.num_edges_ << std::endl;

  std::cout << "Reducing graph..." << std::endl;
  std::clock_t begin = std::clock();
  road_network.reduce_to_largest_connected_component();
  std::clock_t end = std::clock();
  double lcc_secs = double(end - begin) / CLOCKS_PER_SEC;
  std::cout << "LLC reduction: " << lcc_secs << "sec" << std::endl;

  std::cout << "LCC # of nodes: " << road_network.graph_.size() << std::endl;
  std::cout << "LCC # of edges: " << road_network.num_edges_ << std::endl;

  Dijkstras dijkstra = Dijkstras(road_network);
  double total_query_times;
  std::cout << std::fixed << std::setprecision(8);
  for (int i = 0; i < 100; i++) {
    Node const& origin = road_network.get_rand_node();
    Node const& target = road_network.get_rand_node();

    std::clock_t begin = std::clock();
    DistanceMap distances;
    Weight result = dijkstra.search(origin.id_, target.id_, distances);
    std::clock_t end = std::clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    total_query_times += elapsed_secs;
    std::cout << "Elapsed ms: " << elapsed_secs * MS_IN_SEC << std::endl;

    std::cout << "https://www.google.com/maps/dir/" << origin.lat_ << ","
              << origin.lng_ << "/" << target.lat_ << "," << target.lng_
              << std::endl;
    std::cout << "time: " << (result / MS_IN_SEC) / 60 << "m"
              << (result / MS_IN_SEC) % 60 << "s" << std::endl;
    std::cout << std::endl;
  }

  std::cout << std::endl;
  std::cout << "Average query time ms: "
            << (total_query_times / 100.0) * MS_IN_SEC << std::endl;

  return 0;
}
