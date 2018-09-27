#ifndef ROAD_NETWORK_H_
#define ROAD_NETWORK_H_

#include <string>
#include <unordered_map>
#include <vector>

// Represents a directed edge between two nodes.
class Edge {
public:
  Edge(int head_node_id, int cost);

  int head_node_osm_id_;
  // Travel time in seconds.
  int cost_;
};

class Node {
public:
  Node(int osm_id, float lat, float lng);

  int osm_id_;
  float lat_;
  float lng_;
  std::vector<Edge> outgoing_edges_;
};

class RoadNetwork {
public:
  RoadNetwork();

  void add_node(int osm_id, float lat, float lng);

  void add_edge(int tail_id, int head_id, int cost);

  // Return a string representation of the graph for debugging/testing.
  // In the format:
  //   "num_nodes num_edges { node_1_id (edge_1, edge_2, ) }, ..."
  std::string as_string();

  // Accepts an OSM file in XML format, parses it to construct the graph.
  void load_from_osm_file(std::string file_name);

  /* private: */
  int num_nodes_;
  int num_edges_;

  std::unordered_map<int, Node *> graph_;
  std::vector<Node *> nodes_;
};

#endif // ROAD_NETWORK_H_
