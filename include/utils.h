#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "graph_types.h"

// Return a Google Maps link for directions between two nodes, for debugging.
const std::string get_gmaps_directions_link(Node const& n1, Node const& n2) {
  return "https://www.google.com/maps/dir/" + std::to_string(n1.lat_) + "," +
         std::to_string(n1.lng_) + "/" + std::to_string(n2.lat_) + "," +
         std::to_string(n2.lng_);
}

#endif // UTILS_H_
