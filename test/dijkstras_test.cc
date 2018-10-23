#include <functional>
#include <queue>

#include "catch.hpp"

#include "../src/dijkstras.h"
#include "../src/graph_types.h"
#include "../src/road_network.h"

using MinHeapPriorityQueue = std::priority_queue<
    WeightedNode,
    std::vector<WeightedNode>,
    std::greater<std::vector<WeightedNode>::value_type>>;

// Construct a shared graph for testing:
//
// 1 -> 2
// |
// - -> 3 -> 4
// |    |
// 5 <-----> 6
//
// 7 --> 8
RoadNetwork default_road_network() {
  RoadNetwork road_network = RoadNetwork();

  road_network.add_node(1, 0.0, 0.0);
  road_network.add_node(2, 0.0, 0.0);
  road_network.add_node(3, 0.0, 0.0);
  road_network.add_node(4, 0.0, 0.0);
  road_network.add_node(5, 0.0, 0.0);
  road_network.add_node(6, 0.0, 0.0);
  road_network.add_node(7, 0.0, 0.0);
  road_network.add_node(8, 0.0, 0.0);

  road_network.add_edge(1, 2, 1);
  road_network.add_edge(1, 3, 1);
  road_network.add_edge(1, 5, 3);
  road_network.add_edge(3, 4, 1);
  road_network.add_edge(3, 5, 1);
  road_network.add_edge(3, 6, 1);
  road_network.add_edge(7, 8, 1);

  return road_network;
}

TEST_CASE("Verify default road_network", "[Dijkstra]") {
  RoadNetwork road_network = default_road_network();
  REQUIRE(
      road_network.as_string() ==
      "8 14 { 1 (2, 3, 5, ) } { 2 (1, ) } { 3 (1, 4, 5, 6, ) } { 4 (3, ) } { 5 "
      "(1, 3, ) } { 6 (3, ) } { 7 (8, ) } { 8 (7, ) }");
}

TEST_CASE("Min-heap priority queue", "[DS]") {
  MinHeapPriorityQueue pq;
  pq.emplace(WeightedNode(1, 1));
  pq.emplace(WeightedNode(2, 2));
  pq.emplace(WeightedNode(3, 3));

  REQUIRE(pq.top().node_id_ == 1);
  pq.pop();
  REQUIRE(pq.top().node_id_ == 2);
  pq.pop();
  REQUIRE(pq.top().node_id_ == 3);
  pq.pop();
}

TEST_CASE("Search w -1 target marks visited_nodes_", "[Dijkstra]") {
  RoadNetwork road_network = default_road_network();

  Dijkstras dijkstra = Dijkstras(road_network);
  dijkstra.search(1, -1);
  dijkstra.set_round(1);
  dijkstra.search(7, -1);

  for (auto it : dijkstra.visited_nodes_) {
    // Nodes 7 + 8 are disconnected, should be marked as second round
    if (it.first == 7 or it.first == 8) {
      REQUIRE(it.second == 1);
      // All other nodes are part of the LCC
    } else {
      REQUIRE(it.second == 0);
    }
  }
}

TEST_CASE("Search finds path", "[Dijkstra]") {
  RoadNetwork road_network = default_road_network();

  Dijkstras dijkstra = Dijkstras(road_network);
  Weight result;

  result = dijkstra.search(1, 5);
  REQUIRE(result == 2);

  result = dijkstra.search(3, 5);
  REQUIRE(result == 1);
}

TEST_CASE("Disconnected nodes return INF_WEIGHT", "[Dijkstra]") {
  RoadNetwork road_network = default_road_network();

  Dijkstras dijkstra = Dijkstras(road_network);
  Weight result;

  result = dijkstra.search(1, 8);
  REQUIRE(result == INF_WEIGHT);
}
