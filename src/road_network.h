#ifndef ROAD_NETWORK_H_
#define ROAD_NETWORK_H_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

// Represents a directed edge between two nodes.
class Edge {
public:
  Edge(int64_t head_node_id, int cost);

  int64_t head_node_osm_id_;
  // Travel time in seconds.
  int cost_;
};

class Node {
public:
  Node(int64_t osm_id, double lat, double lng);

  int osm_id_;
  double lat_;
  double lng_;
  std::vector<Edge> outgoing_edges_;
};

class RoadNetwork {
public:
  RoadNetwork();

  void add_node(int64_t osm_id, double lat, double lng);

  void add_edge(int64_t tail_id, int64_t head_id, float road_speed);

  int calculate_travel_seconds(Node* tail_node, Node
  // Return a string representation of the graph for debugging/testing.
  // In the format:
  //   "num_nodes num_edges { node_1_id (edge_1, edge_2, ) }, ..."
  std::string as_string();

  // Accepts an OSM file in XML format, parses it to construct the graph.
  void load_from_osm_file(const char* file_name);

  /* private: */
  int num_nodes_;
  int num_edges_;

  std::unordered_map<int64_t, Node*> graph_;
  std::vector<Node*> nodes_;
};

#endif // ROAD_NETWORK_H_
