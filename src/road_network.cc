#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "road_network.h"
#include "road_speeds.h"

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

std::string RoadNetwork::as_string() {
  std::string output =
      std::to_string(num_nodes_) + " " + std::to_string(num_edges_);
  for (Node *node : nodes_) {
    output += " { " + std::to_string(node->osm_id_) + " (";
    for (Edge edge : node->outgoing_edges_) {
      output += std::to_string(edge.head_node_osm_id_) + ", ";
    }
    output += ") }";
  }
  return output;
}

void RoadNetwork::load_from_osm_file(std::string file_name) {
  std::string line;
  std::ifstream osm_file(file_name);

  if (!osm_file.is_open()) {
    return;
  }

  std::smatch match;
  std::regex node_regex("<"
                        "node\\sid=\"(\\d+)\".*lat=\"(-?\\d+\\.\\d+)\"\\s+lon="
                        "\"(-?\\d+\\.\\d+)\"");

  std::regex start_way_regex("<way.*");
  std::regex way_ref_regex("<nd ref=\"(\\d+)\"");
  std::regex way_highway_tag_regex("<tag\\sk=\"highway\"\\sv=\"([a-z_]+)\"");

  std::string way_type("");
  bool is_within_way = false;
  std::vector<int> curr_nodes;
  while (std::getline(osm_file, line)) {
    if (std::regex_search(line, match, start_way_regex)) {
      is_within_way = true;
    } else if (is_within_way) {
      if (std::regex_search(line, match, way_ref_regex)) {
        curr_nodes.push_back(std::stol(match.str(1)));
      } else if (std::regex_search(line, match, way_highway_tag_regex)) {
        way_type = match.str(1);
      } else {
        if (way_type != "") {
          way_type.clear();
          is_within_way = false;

          std::string log("");
          for (int node : curr_nodes) {
            log += std::to_string(node) + " ";
          }
          std::cout << log << std::endl;
          // TODO - actually add the edges
        }
      }
    } else if (std::regex_search(line, match, node_regex)) {
      int node_id = std::stoi(match.str(1));
      int lat = std::stoi(match.str(2));
      int lng = std::stoi(match.str(3));
      add_node(node_id, lat, lng);
    }
  }
  osm_file.close();
}
