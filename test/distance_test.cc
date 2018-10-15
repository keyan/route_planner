#include <cstdlib>
#include "catch.hpp"

#include "../src/distance.h"

// Can't really compare double/float vals, so use arbitrary small val.
static const double EPSILON = 0.00001;

TEST_CASE("Haversine matches other libs", "[]") {
  double dist_km;

  dist_km = haversine(44.453905, -73.200317, 44.453905, -73.200386);
  REQUIRE(std::abs(dist_km - 0.005477) <= EPSILON);

  dist_km = haversine(44.455595, -73.218522, 44.455595,-73.218655);
  REQUIRE(std::abs(dist_km - 0.01056) <= EPSILON);
}
