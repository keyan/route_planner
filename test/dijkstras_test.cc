#include <functional>
#include <queue>

#include "catch.hpp"

#include "../include/dijkstras.h"
#include "../include/graph_types.h"
#include "../include/road_network.h"
#include "util/constants.h"

using MinHeapPriorityQueue = std::priority_queue<
    WeightedNode,
    std::vector<WeightedNode>,
    std::greater<std::vector<WeightedNode>::value_type>>;

// Construct a shared graph for testing:
//
// 0
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

  road_network.add_node(0, 0, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(1, 1, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(2, 2, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(3, 3, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(4, 4, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(5, 5, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(6, 6, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(7, 7, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(8, 8, UNUSED_LAT_LNG, UNUSED_LAT_LNG);

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
      "9 14 { 0 () } { 1 (2, 3, 5, ) } { 2 (1, ) } { 3 (1, 4, 5, 6, ) } { 4 (3,"
      " ) } { 5 (1, 3, ) } { 6 (3, ) } { 7 (8, ) } { 8 (7, ) }");
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
  DistanceMap distances;
  dijkstra.search(0, -1, distances);
  distances.clear();
  dijkstra.set_round(1);
  dijkstra.search(1, -1, distances);
  distances.clear();
  dijkstra.set_round(2);
  dijkstra.search(7, -1, distances);

  NodeID i = 0;
  for (auto it = dijkstra.visited_nodes_.begin();
       it != dijkstra.visited_nodes_.end(); it++, i++) {
    // Nodes 7 + 8 are disconnected, should be marked as round 2
    if (i == 7 or i == 8) {
      REQUIRE(*it == 2);
    } else if (i == 0) {
      REQUIRE(*it == 0);
      // All other nodes are part of the LCC
    } else {
      REQUIRE(*it == 1);
    }
  }
}

TEST_CASE("Search finds path", "[Dijkstra]") {
  RoadNetwork road_network = default_road_network();

  Dijkstras dijkstra = Dijkstras(road_network);
  Weight result;

  DistanceMap distances;
  result = dijkstra.search(1, 5, distances);
  distances.clear();
  REQUIRE(result == 2);

  result = dijkstra.search(3, 5, distances);
  distances.clear();
  REQUIRE(result == 1);
}

TEST_CASE("Disconnected nodes return INF_WEIGHT", "[Dijkstra]") {
  RoadNetwork road_network = default_road_network();

  Dijkstras dijkstra = Dijkstras(road_network);
  Weight result;

  DistanceMap distances;
  result = dijkstra.search(1, 8, distances);
  REQUIRE(result == INF_WEIGHT);
}
