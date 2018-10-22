#include "catch.hpp"
#include <cstdlib>

#include "../src/road_network.h"
#include "util/constants.h"

TEST_CASE("Empty graph representation", "[]") {
  RoadNetwork road_network = RoadNetwork();

  REQUIRE(road_network.as_string() == "0 0");
}

TEST_CASE("Add nodes", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(2, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  REQUIRE(road_network.as_string() == "2 0 { 1 () } { 2 () }");
}

TEST_CASE("Add edges", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(2, UNUSED_LAT_LNG, UNUSED_LAT_LNG);

  road_network.add_edge(1, 2, 30);
  REQUIRE(road_network.as_string() == "2 1 { 1 (2, ) } { 2 () }");

  road_network.add_edge(1, 2, 15);
  road_network.add_edge(2, 1, 20);
  road_network.add_edge(2, 1, 10);
  REQUIRE(road_network.as_string() == "2 4 { 1 (2, 2, ) } { 2 (1, 1, ) }");
}

TEST_CASE("Add ways produces correct graph", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(2, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(3, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  REQUIRE(road_network.as_string() == "3 0 { 1 () } { 2 () } { 3 () }");

  road_network.add_way({1, 2, 3}, "trunk");
  road_network.add_way({2, 1}, "trunk");
  REQUIRE(
      road_network.as_string() == "3 3 { 1 (2, ) } { 2 (3, 1, ) } { 3 () }");
}

TEST_CASE("Add ways produces edges with correct weight", "[]") {
  RoadNetwork road_network = RoadNetwork();

  road_network.add_node(1, POINT_1_LAT, POINT_1_LNG);
  road_network.add_node(2, POINT_2_LAT, POINT_2_LNG);

  road_network.add_way({1, 2}, "service");

  Weight expected_weight = int(((POINTS_DISTANCE_KM / 5.0) * MS_IN_HOUR) + 0.5);
  REQUIRE(
      std::abs(
          road_network.graph_.at(1).outgoing_edges_[0].weight_ -
          expected_weight) <= EPSILON);
}

TEST_CASE("Calculate travel time accurate", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(2, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
}

TEST_CASE("Reduce to LCC", "[]") {
  // 1 -> 2
  //
  // 3 -> 4
  //
  // 5 -> 6 -> 7
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(2, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(3, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(4, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(5, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(6, UNUSED_LAT_LNG, UNUSED_LAT_LNG);
  road_network.add_node(7, UNUSED_LAT_LNG, UNUSED_LAT_LNG);

  road_network.add_edge(1, 2, 1);
  road_network.add_edge(3, 4, 1);
  road_network.add_edge(5, 6, 1);
  road_network.add_edge(6, 7, 1);
  REQUIRE(
      road_network.as_string() == "7 4 { 1 (2, ) } { 2 () } { 3 (4, ) } { 4 () "
                                  "} { 5 (6, ) } { 6 (7, ) } { 7 () }");

  road_network.reduce_to_largest_connected_component();

  REQUIRE(road_network.as_string() == "3 2 { 5 (6, ) } { 6 (7, ) } { 7 () }");
}

TEST_CASE("Load XML OSM file", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file("test/data/test.osm");

  REQUIRE(
      road_network.as_string() ==
      "8 7 { 1 (2, 3, 5, ) } { 2 () } { 3 (4, 5, 6, ) } { 4 () } { 5 () } { 6 "
      "() } { 7 (8, ) } { 8 () }");
}

TEST_CASE("Reduce XML graph to LCC", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file("test/data/test.osm");

  REQUIRE(
      road_network.as_string() ==
      "8 7 { 1 (2, 3, 5, ) } { 2 () } { 3 (4, 5, 6, ) } { 4 () } { 5 () } { 6 "
      "() } { 7 (8, ) } { 8 () }");

  road_network.reduce_to_largest_connected_component();

  REQUIRE(
      road_network.as_string() ==
      "6 6 { 1 (2, 3, 5, ) } { 2 () } { 3 (4, 5, 6, ) } { 4 () } { 5 () } { 6 "
      "() }");

  // nodes_ vector is updated post-LCC filtering
  std::vector<int> expected_node_ids = {1, 2, 3, 4, 5, 6};
  for (Node* node : road_network.nodes_) {
    REQUIRE(
        (std::find(
             std::begin(expected_node_ids), std::end(expected_node_ids),
             node->osm_id_) != std::end(expected_node_ids)) == true);
  }
}
