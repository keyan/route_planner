#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

#include "tinyxml2.h"

#include "constants.h"
#include "dijkstras.h"
#include "distance.h"
#include "graph_types.h"
#include "road_network.h"
#include "road_speeds.h"

RoadNetwork::RoadNetwork() {
  num_nodes_ = 0;
  num_edges_ = 0;
}

void RoadNetwork::add_node(NodeID osm_id, double lat, double lng) {
  if (graph_.find(osm_id) == graph_.end()) {
    Node* const new_node = new Node(osm_id, lat, lng);
    graph_.emplace(osm_id, new_node);
    nodes_.push_back(new_node);
    num_nodes_++;
  }
}

void RoadNetwork::add_edge(EdgeID tail_id, NodeID head_id, Weight weight) {
  graph_.at(tail_id)->outgoing_edges_.push_back(Edge(head_id, weight));
  num_edges_++;
}

void RoadNetwork::add_way(NodeIDList node_ids, std::string highway_type) {
  float road_speed_kmh;
  RoadSpeeds::const_iterator search = road_speeds.find(highway_type);
  if (search == road_speeds.end()) {
    return;
  } else {
    road_speed_kmh = search->second;
  }
  if (node_ids.size() < 2) {
    return;
  }
  NodeIDList::const_iterator it = node_ids.begin();
  EdgeID tail_id = *it;
  NodeID head_id;
  for (it = std::next(it); it != node_ids.end(); ++it) {
    head_id = *it;
    Weight weight = calculate_travel_seconds(tail_id, head_id, road_speed_kmh);
    add_edge(tail_id, head_id, weight);
    tail_id = head_id;
  }
}

Weight RoadNetwork::calculate_travel_seconds(
    EdgeID tail_id, NodeID head_id, float road_speed_kmh) {
  Node* tail_node = graph_.at(tail_id);
  Node* head_node = graph_.at(head_id);
  double distance_km = haversine(
      tail_node->lat_, tail_node->lng_, head_node->lat_, head_node->lng_);
  // Rounds to nearest second, maybe this should be in ms instead?
  Weight travel_seconds =
      int(((distance_km / road_speed_kmh) * SECONDS_IN_HOUR) + 0.5);
  return travel_seconds;
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

void RoadNetwork::reduce_to_largest_connected_component() {
  Dijkstras dijkstra = Dijkstras(*this);

  // Successive graph search, marking each node with the round that it was
  // visited.
  int round = 0;
  for (Node* node : nodes_) {
    if (dijkstra.visited_nodes_[node->osm_id_] == -1) {
      dijkstra.search(node->osm_id_, -1);
      dijkstra.set_round(++round);
    }
  }

  std::vector<NodeIDSet> nodes_by_round;
  for (int i = 0; i < round; ++i) {
    nodes_by_round.push_back({0});
  }

  for (auto it : dijkstra.visited_nodes_) {
    nodes_by_round[it.second].insert(it.first);
  }

  // Fetch the round with the most visited nodes
  std::vector<NodeIDSet>::iterator it = max_element(
      nodes_by_round.begin(), nodes_by_round.end(),
      [](NodeIDSet& A, NodeIDSet& B) { return A.size() < B.size(); });

  filter_nodes(*it);
}

void RoadNetwork::filter_nodes(NodeIDSet include_nodes) {
  std::vector<Node*>::iterator it = nodes_.begin();
  // Mutate the nodes_ vector while iterating
  while (it != nodes_.end()) {
    if (include_nodes.count((*it)->osm_id_) == 0) {
      graph_.erase((*it)->osm_id_);
      // erase() already returns iterator to next element
      it = nodes_.erase(it);
    } else {
      ++it;
    }
  }
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
      // Way have both a nested list of 2+ nodes, and at least one "tag" child
      // that we do care about.
    } else if (osm_element_type == "way") {
      // Only "highway" types are used for road routing, use empty string to
      // indicate the way is not valid.
      std::string highway_type("");
      std::vector<NodeID> nodes_in_way;
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
      if (highway_type != "") {
        add_way(nodes_in_way, highway_type);
      }
      highway_type = "";
      nodes_in_way.clear();
    }
  }
}
