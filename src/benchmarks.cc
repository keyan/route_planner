#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "alt.h"
#include "benchmarks.h"
#include "constants.h"
#include "graph_types.h"
#include "road_network.h"

using std::cout;
using std::endl;

void run_benchmarks() {
  // Seed random generator
  std::srand(std::time(0));

  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file("data/burlington.osm");
  // road_network.load_from_osm_file("data/saarland.osm");
  cout << "Number of nodes: " << road_network.graph_.size() << endl;
  cout << "Number of edges: " << road_network.num_edges_ << endl;

  cout << "Reducing graph..." << endl;
  std::clock_t begin = std::clock();
  road_network.reduce_to_largest_connected_component();
  std::clock_t end = std::clock();
  double lcc_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "LLC reduction: " << lcc_secs << "sec" << endl;

  cout << "LCC # of nodes: " << road_network.graph_.size() << endl;
  cout << "LCC # of edges: " << road_network.num_edges_ << endl;

  std::vector<std::pair<Node const&, Node const&>> search_cases;
  for (int i = 0; i < 10; i++) {
    Node const& origin = road_network.get_rand_node();
    Node const& target = road_network.get_rand_node();

    search_cases.emplace_back(origin, target);
  }

  cout << std::fixed << std::setprecision(6);

  cout << "Running plain dijkstras: " << endl;
  Dijkstras dijkstra = Dijkstras(road_network);
  double total_query_times = 0.0;
  for (auto search_case : search_cases) {
    Node const& origin = search_case.first;
    Node const& target = search_case.second;

    std::clock_t begin = std::clock();
    DistanceMap distances;
    Weight result = dijkstra.search(origin.id_, target.id_, distances);
    std::clock_t end = std::clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    total_query_times += elapsed_secs;
    cout << "Elapsed ms: " << elapsed_secs * MS_IN_SEC << endl;
    cout << "Settled nodes: " << dijkstra.shortest_path_tree_.size() << endl;

    cout << "https://www.google.com/maps/dir/" << origin.lat_ << ","
         << origin.lng_ << "/" << target.lat_ << "," << target.lng_ << endl;
    cout << "time: " << (result / MS_IN_SEC) / 60 << "m"
         << (result / MS_IN_SEC) % 60 << "s" << endl;
  }

  cout << endl;
  cout << "Average query time ms: "
       << (total_query_times / search_cases.size()) * MS_IN_SEC << endl;
  cout << endl;

  cout << "Running ALT: " << endl;
  ALT alt = ALT(road_network, dijkstra);
  total_query_times = 0.0;
  for (auto search_case : search_cases) {
    Node const& origin = search_case.first;
    Node const& target = search_case.second;

    std::clock_t begin = std::clock();
    DistanceMap distances;
    Weight result = alt.search(origin.id_, target.id_);
    std::clock_t end = std::clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    total_query_times += elapsed_secs;
    cout << "Elapsed ms: " << elapsed_secs * MS_IN_SEC << endl;
    cout << "Settled nodes: " << dijkstra.shortest_path_tree_.size() << endl;

    cout << "https://www.google.com/maps/dir/" << origin.lat_ << ","
         << origin.lng_ << "/" << target.lat_ << "," << target.lng_ << endl;
    cout << "time: " << (result / MS_IN_SEC) / 60 << "m"
         << (result / MS_IN_SEC) % 60 << "s" << endl;
  }

  cout << endl;
  cout << "Average query time ms: "
       << (total_query_times / search_cases.size()) * MS_IN_SEC << endl;
  cout << endl;
}
