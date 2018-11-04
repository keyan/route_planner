#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "constants.h"
#include "distance.h"
#include "graph_types.h"

const float MAX_NEIGHBOR_THRESHOLD_KM = 0.1;

// Return a Google Maps link for directions between two nodes, for debugging.
const std::string get_gmaps_directions_link(Node const& n1, Node const& n2) {
  return "https://www.google.com/maps/dir/" + std::to_string(n1.lat_) + "," +
         std::to_string(n1.lng_) + "/" + std::to_string(n2.lat_) + "," +
         std::to_string(n2.lng_);
}

// Given a MS value, construct a UX friendly string in the form XXmXXs.
const std::string ms_to_mins_and_secs(Weight ms) {
  return std::to_string((ms / MS_IN_SEC) / 60) + "m" +
         std::to_string((ms / MS_IN_SEC) % 60) + "s";
}

// Given a position, find the closest node in the network, or -1 if no suitable
// result could be found. Currently does a naive linear scan, eventually I'd
// like to use an R-tree, KD-tree, or VP-tree, e.g.:
// github.com/RoutingKit/RoutingKit/blob/master/src/geo_position_to_node.cpp
const NodeID geo_position_to_node(RoadNetwork& road_network, LatLng pos) {
  NodeID best_node_id;
  float closest_node_km = INF_DISTANCE;

  for (Node node : road_network.graph_) {
    float distance = haversine(pos.lat_, pos.lng_, node.lat_, node.lng_);
    if (distance < closest_node_km) {
      best_node_id = node.id_;
      closest_node_km = distance;
    }
  }

  if (closest_node_km <= MAX_NEIGHBOR_THRESHOLD_KM) {
    return best_node_id;
  } else {
    return -1;
  }
}

#endif // UTILS_H_
