#ifndef ROAD_NETWORK_H_
#define ROAD_NETWORK_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "graph_types.h"

class RoadNetwork {
public:
  RoadNetwork();

  void add_node(NodeID osm_id, double lat, double lng);
  void add_edge(NodeID tail_id, NodeID head_id, Weight weight);

  // Add an OSM way to the graph.
  //
  // Ways are represented as a list of N (min 2) node_ids, where the resulting
  // number of edges is N - 1. All the edges in the way are associated with the
  // same highway_type.
  void add_way(NodeIDList node_ids, std::string highway_type);

  // Return a string representation of the graph for debugging/testing.
  //
  // In the format:
  //   "num_nodes num_edges { node_1_id (edge_1, edge_2, ) }, ..."
  std::string as_string();

  // Accepts an OSM file in XML format, parses it to construct the graph.
  void load_from_osm_file(const char* file_name);

  // Reduce graph to largest connected component.
  void reduce_to_largest_connected_component();

  // Return a random node id from the graph.
  const Node& get_rand_node();

  int num_nodes_;
  int num_edges_;

  std::unordered_map<NodeID, Node> graph_;
  std::vector<Node*> nodes_;

private:
  // Given two nodes, return the time in seconds to travel between them.
  Weight calculate_travel_ms(
      NodeID tail_node_id, NodeID head_node_id, float road_speed_kmh);

  // Given a list of nodes to include in the graph, filter all others out.
  void filter_nodes(NodeIDSet);
};

#endif // ROAD_NETWORK_H_
