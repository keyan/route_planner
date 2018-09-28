#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "tinyxml2.h"

#include "distance.h"
#include "road_network.h"
#include "road_speeds.h"

Edge::Edge(int64_t head_node_id, int cost) {
  head_node_osm_id_ = head_node_id;
  cost_ = cost;
}

Node::Node(int64_t osm_id, double lat, double lng) {
  osm_id_ = osm_id;
  lat_ = lat;
  lng_ = lng;
}

RoadNetwork::RoadNetwork() {
  num_nodes_ = 0;
  num_edges_ = 0;
}

void RoadNetwork::add_node(int64_t osm_id, double lat, double lng) {
  if (graph_.find(osm_id) == graph_.end()) {
    Node* new_node = new Node(osm_id, lat, lng);
    graph_.emplace(osm_id, new_node);
    nodes_.push_back(new_node);
    num_nodes_++;
  }
}

void RoadNetwork::add_edge(int64_t tail_id, int64_t head_id, float road_speed) {
  Node* tail_node = nodes_.at(tail_id);
  Node* head_node = nodes_.at(head_id);
  double distance = haversine(tail_node->lat_, tail_node->lng_, head_node->lat_, head_node->lng_);
  int cost = 0;
  graph_.at(tail_id)->outgoing_edges_.push_back(Edge(head_id, cost));
  num_edges_++;
}

int RoadNetwork::calculate_travel_seconds(int64_t tail_id, int64_t head_id) {

}

std::string RoadNetwork::as_string() {
  std::string output =
      std::to_string(num_nodes_) + " " + std::to_string(num_edges_);
  for (Node* node : nodes_) {
    output += " { " + std::to_string(node->osm_id_) + " (";
    for (Edge edge : node->outgoing_edges_) {
      output += std::to_string(edge.head_node_osm_id_) + ", ";
    }
    output += ") }";
  }
  return output;
}

void RoadNetwork::load_from_osm_file(const char* file_name) {
  tinyxml2::XMLDocument doc;
  doc.LoadFile(file_name);

  const tinyxml2::XMLElement* osm = doc.FirstChildElement("osm")->ToElement();
  if (!osm) {
    std::cout << "OSM file read failure" << std::endl;
    return;
  }

  // Iterate through each OSM data type these are:
  //   - node
  //   - way
  //   - relation (which we ignore)
  for (const tinyxml2::XMLNode* element = osm->FirstChildElement(); element;
       element = element->NextSibling()) {
    const tinyxml2::XMLElement* e = element->ToElement();

    std::string osm_element_type(e->Value());
    // Node's are simply one element, some nodes have "tag" child elements we
    // do not need and are ignored.
    if (osm_element_type == "node") {
      add_node(
          e->Int64Attribute("id"), e->DoubleAttribute("lon"),
          e->DoubleAttribute("lon"));
      // Way's have both a nested list of 2+ nodes, and at least one "tag" child
      // that we do care about.
    } else if (osm_element_type == "way") {
      // Only "highway" types are used for road routing, use empty string to
      // indicate the way is not valid.
      std::string highway_type("");
      std::vector<int64_t> nodes_in_way;
      for (const tinyxml2::XMLNode* way_item = e->FirstChildElement(); way_item;
           way_item = way_item->NextSibling()) {
        const tinyxml2::XMLElement* w = way_item->ToElement();
        std::string way_element_type(w->Value());
        if (way_element_type == "nd") {
          nodes_in_way.push_back(w->Int64Attribute("ref"));
        } else if (way_element_type == "tag") {
          std::string key(w->Attribute("k"));
          std::string value(w->Attribute("v"));
          if (key == "highway") {
            highway_type = value;
          }
        }
      }
      // After processing all element for the way, assess whether to keep.
      if (highway_type != "" && nodes_in_way.size() > 1) {
        float road_speed_kmh = road_speeds.at(highway_type);
        for (int i = 1; i < nodes_in_way.size() - 1; i++) {
          add_edge(nodes_in_way[i], nodes_in_way[i+1], road_speed_kmh);
        }
      }
      highway_type = "";
      nodes_in_way.clear();
    }
  }
}
