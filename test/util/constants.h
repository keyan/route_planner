#ifndef TEST_CONSTANTS_H_
#define TEST_CONSTANTS_H_

#include "../src/distance.h"

// Can't really compare double/float vals, so use arbitrary small val
double const  EPSILON = 0.00001;

// Used to indicate an irrelevant float value
double const UNUSED_LAT_LNG = 0.0;

// Two points which are 1KM apart using our haversine method
double const POINT_1_LAT = 44.4806118;
double const POINT_1_LNG = -73.2068952;
double const POINT_2_LAT = 44.3059999;
double const POINT_2_LNG = -73.2068902;

double const POINTS_DISTANCE_KM = haversine(POINT_1_LAT, POINT_1_LNG, POINT_2_LAT, POINT_2_LNG);

#endif // TEST_CONSTANTS_H_
