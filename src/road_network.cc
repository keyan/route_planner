#include <algorithm>
#include <cstdint>
#include <cstdlib>
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

RoadNetwork::RoadNetwork() { num_edges_ = 0; }

void RoadNetwork::add_node(
    NodeID internal_id, NodeID osm_id, double lat, double lng) {
  if (id_to_osm_id_.find(osm_id) == id_to_osm_id_.end()) {
    graph_.emplace_back(Node(internal_id, osm_id, lat, lng));
    id_to_osm_id_[osm_id] = internal_id;
  }
}

void RoadNetwork::add_edge(EdgeID tail_id, NodeID head_id, Weight weight) {
  // Add an edge for both directions, but only increment edge count by 1.
  // Edge count is used to track number of undirected edges, although
  // internally we are representing each undirected edge as two directed edges.
  graph_.at(tail_id).outgoing_edges_.push_back(Edge(head_id, weight));
  graph_.at(head_id).outgoing_edges_.push_back(Edge(tail_id, weight));
  num_edges_ += 2;
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

  NodeID internal_tail_id = id_to_osm_id_[tail_id];
  NodeID internal_head_id;
  for (it = std::next(it); it != node_ids.end(); ++it) {
    head_id = *it;
    internal_head_id = id_to_osm_id_[head_id];

    Weight weight =
        calculate_travel_ms(internal_tail_id, internal_head_id, road_speed_kmh);
    add_edge(internal_tail_id, internal_head_id, weight);
    tail_id = head_id;
    internal_tail_id = internal_head_id;
  }
}

const Node& RoadNetwork::get_rand_node() {
  int idx = std::rand() % graph_.size() + 1;
  Node const& rand_node = graph_[idx];

  return rand_node;
}

Weight RoadNetwork::calculate_travel_ms(
    EdgeID tail_id, NodeID head_id, float road_speed_kmh) {
  Node const& tail_node = graph_[tail_id];
  Node const& head_node = graph_[head_id];
  double distance_km =
      haversine(tail_node.lat_, tail_node.lng_, head_node.lat_, head_node.lng_);
  // Rounds to nearest ms
  Weight travel_ms = int(((distance_km / road_speed_kmh) * MS_IN_HOUR) + 0.5);
  return travel_ms;
}

std::string RoadNetwork::as_string() {
  std::string output =
      std::to_string(graph_.size()) + " " + std::to_string(num_edges_);
  for (Node const& node : graph_) {
    output += " { " + std::to_string(node.id_) + " (";
    for (Edge const& edge : node.outgoing_edges_) {
      output += std::to_string(edge.head_node_id_) + ", ";
    }
    output += ") }";
  }
  return output;
}

void RoadNetwork::reduce_to_largest_connected_component() {
  Dijkstras dijkstra = Dijkstras(*this);

  // Successive graph search, marking each node with the round that it was
  // visited.
  int64_t round = 0;
  for (Node const& node : graph_) {
    if (dijkstra.visited_nodes_[node.id_] == -1) {
      DistanceMap distances;
      dijkstra.search(node.id_, -1, distances);
      dijkstra.set_round(++round);
    }
  }

  // Initialize empty set entry for each round
  std::vector<NodeIDSet> nodes_by_round;
  for (int64_t i = 0; i < round; ++i) {
    nodes_by_round.push_back({});
  }

  // Group node_ids by round
  NodeID i = 0;
  for (auto it = dijkstra.visited_nodes_.begin();
       it != dijkstra.visited_nodes_.end(); it++, i++) {
    nodes_by_round[*it].insert(i);
  }

  // Fetch the round with the most visited nodes
  std::vector<NodeIDSet>::iterator it = std::max_element(
      nodes_by_round.begin(), nodes_by_round.end(),
      [](NodeIDSet& A, NodeIDSet& B) { return A.size() < B.size(); });

  filter_nodes(*it);
}

void RoadNetwork::filter_nodes(NodeIDSet include_nodes) {
  std::unordered_map<NodeID, NodeID> old_id_to_new_id;
  std::vector<Node> new_graph;

  NodeID remapped_id = 0;
  for (auto it = graph_.begin(); it != graph_.end(); it++) {
    if (include_nodes.count((*it).id_) != 0) {
      Node new_node = *it;
      new_node.id_ = remapped_id;
      old_id_to_new_id[(*it).id_] = new_node.id_;
      id_to_osm_id_[(*it).id_] = new_node.osm_id_;
      new_graph.push_back(new_node);
      remapped_id++;
    }
  }

  // Remap edge pointers using new ids
  num_edges_ = 0;
  for (Node& node : new_graph) {
    for (Edge& edge : node.outgoing_edges_) {
      edge.head_node_id_ = old_id_to_new_id[edge.head_node_id_];
      num_edges_++;
    }
  }

  graph_.swap(new_graph);
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
  NodeID curr_node_id = 0;
  for (const tinyxml2::XMLNode* element = osm->FirstChildElement(); element;
       element = element->NextSibling()) {
    const tinyxml2::XMLElement* e = element->ToElement();

    std::string osm_element_type(e->Value());
    // Node's are simply one element, some nodes have "tag" child elements we
    // do not need and are ignored.
    if (osm_element_type == "node") {
      add_node(
          curr_node_id, e->Int64Attribute("id"), e->DoubleAttribute("lat"),
          e->DoubleAttribute("lon"));
      curr_node_id++;
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
