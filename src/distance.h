#ifndef DISTANCE_H_
#define DISTANCE_H_

#include <math.h>

const static double earth_radius_km = 6372.8;

inline double degree_to_radian(double angle) { return angle * (M_PI / 180.0); }

// Return the great-circle distance between two points in KM.
double haversine(double lat1, double lng1, double lat2, double lng2) {
  double lat_rad1 = degree_to_radian(lat1);
  double lng_rad1 = degree_to_radian(lng1);
  double lat_rad2 = degree_to_radian(lat2);
  double lng_rad2 = degree_to_radian(lng2);

  double diff_lat = lat_rad2 - lat_rad1;
  double diff_lng = lng_rad2 - lng_rad2;

  double computation = asin(sqrt(
      sin(diff_lat / 2) * sin(diff_lat / 2) +
      cos(lat_rad1) * cos(lat_rad2) * sin(diff_lng / 2) * sin(diff_lng / 2)));

  return 2 * earth_radius_km * computation;
}

#endif // DISTANCE_H_
