#include <iostream>
#include <vector>

#include "alt.h"

void ALT::select_landmarks(int num_landmarks) {
  for (int i = 0; i < num_landmarks; i++) {
    landmarks_.push_back(road_network_.get_rand_node().id_);
  }
}

void ALT::precompute_landmark_map() {
  if (landmarks_.empty()) {
    std::cout << "No landmarks selected, cannot do precomputation" << std::endl;
    return;
  }
}
