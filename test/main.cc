#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../src/road_network.h"

TEST_CASE("Empty graph representation", "[]") {
  RoadNetwork road_network = RoadNetwork();

  REQUIRE(road_network.as_string() == "0 0");
}

TEST_CASE("Add nodes", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, 1.1, 1.1);
  road_network.add_node(2, 2.2, 2.2);
  REQUIRE(road_network.as_string() == "2 0 { 1 () } { 2 () }");
}

TEST_CASE("Add edges", "[]") {
  RoadNetwork road_network = RoadNetwork();
  road_network.add_node(1, 1.1, 1.1);
  road_network.add_node(2, 2.2, 2.2);

  road_network.add_edge(1, 2, 30);
  REQUIRE(road_network.as_string() == "2 1 { 1 (2, ) } { 2 () }");

  road_network.add_edge(1, 2, 15);
  road_network.add_edge(2, 1, 20);
  road_network.add_edge(2, 1, 10);
  REQUIRE(road_network.as_string() == "2 4 { 1 (2, 2, ) } { 2 (1, 1, ) }");
}
