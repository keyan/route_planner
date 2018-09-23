#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "road_network.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;

Edge::Edge(int head_node_id, int cost) {
  head_node_osm_id_ = head_node_id;
  cost_ = cost;
}

Node::Node(int osm_id, float lat, float lng) {
  osm_id_ = osm_id;
  lat_ = lat;
  lng_ = lng;
}

RoadNetwork::RoadNetwork() {
  num_nodes_ = 0;
  num_edges_ = 0;
}

void RoadNetwork::add_node(int osm_id, float lat, float lng) {
  if (graph_.find(osm_id) == graph_.end()) {
    Node *new_node = new Node(osm_id, lat, lng);
    graph_.emplace(osm_id, new_node);
    nodes_.push_back(new_node);
    num_nodes_++;
  }
}

void RoadNetwork::add_edge(int tail_id, int head_id, int cost) {
  graph_.at(tail_id)->outgoing_edges_.push_back(Edge(head_id, cost));
  num_edges_++;
}

string RoadNetwork::as_string() {
  string output = to_string(num_nodes_) + " " + to_string(num_edges_);
  for (Node *node : nodes_) {
    output += " { " + to_string(node->osm_id_) + " (";
    for (Edge edge : node->outgoing_edges_) {
      output += to_string(edge.head_node_osm_id_) + ", ";
    }
    output += ") }";
  }
  return output;
}

void RoadNetwork::load_from_osm_file(string file_name) { ; }
