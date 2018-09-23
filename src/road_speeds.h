#ifndef ROAD_SPEEDS_H_
#define ROAD_SPEEDS_H_

#include <string>
#include <unordered_map>

using RoadSpeeds = std::unordered_map<std::string, float>;

// Fixed arbitrary km/h values for different 'highway' type ways
const RoadSpeeds road_speeds = {
    {"motorway", 110},   {"trunk", 110},       {"primary", 70},
    {"secondary", 60},   {"tertiary", 50},     {"motorway_link", 50},
    {"trunk_link", 50},  {"primary_link", 50}, {"secondary_link", 50},
    {"trunk_link", 50},  {"road", 40},         {"unclassified", 40},
    {"residential", 30}, {"unsurfaced", 30},   {"living_street", 10},
    {"service", 5},
};

#endif // ROAD_SPEEDS_H_
