#ifndef DISTANCE_H_
#define DISTANCE_H_

#include <math.h>

#include "constants.h"

constexpr float INF_DISTANCE = std::numeric_limits<float>::max();

inline double degree_to_radian(double angle) { return angle * M_PI / 180.0; }

// Return the great-circle distance between two points in KM.
inline double haversine(double lat1, double lng1, double lat2, double lng2) {
  double lat_rad1 = degree_to_radian(lat1);
  double lng_rad1 = degree_to_radian(lng1);
  double lat_rad2 = degree_to_radian(lat2);
  double lng_rad2 = degree_to_radian(lng2);

  double diff_lat = lat_rad2 - lat_rad1;
  double diff_lng = lng_rad2 - lng_rad1;

  double u = sin(diff_lat / 2.0);
  double v = sin(diff_lng / 2.0);

  double computation =
      asin(sqrt(u * u + cos(lat_rad1) * cos(lat_rad2) * v * v));

  return 2.0 * EARTH_RADIUS_KM * computation;
}

#endif // DISTANCE_H_
