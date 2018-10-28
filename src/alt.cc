#include <iostream>
#include <vector>

#include "alt.h"

Weight ALT::search(NodeID const& source_node_id, NodeID const& target_node_id) {
  // Precompute the heuristic vector where each index i is the max of
  // abs(dist(i, l) - dist(v, l)) of all landmarks.
  std::vector<Weight> heuristic(road_network_.graph_.size());
  for (std::size_t i = 0; i < road_network_.graph_.size(); ++i) {
    Weight max_weight = 0;
    // for (NodeID const& landmark_node_id : landmarks_) {
    for (std::size_t j = 0; j < landmarks_.size(); ++j) {
      Weight const& current_to_landmark = landmark_distances_[j][i];
      Weight const& target_to_landmark = landmark_distances_[j][target_node_id];

      max_weight = std::max(
          max_weight, std::abs(current_to_landmark - target_to_landmark));
    }
    heuristic[i] = max_weight;
  }
  dijkstras_.set_heuristic(&heuristic);

  DistanceMap distances;
  Weight result = dijkstras_.search(source_node_id, target_node_id, distances);

  return result;
}

void ALT::precompute_landmark_map() {
  std::vector<std::vector<Weight>> landmark_distances(landmarks_.size());
  for (std::size_t i = 0; i < landmarks_.size(); ++i) {
    DistanceMap distances;
    dijkstras_.search(landmarks_[i], -1, distances);

    for (int j = 0; j < road_network_.graph_.size(); j++) {
      landmark_distances[i].push_back(distances[j]);
    }
  }

  landmark_distances_ = landmark_distances;
}

void ALT::select_landmarks(int num_landmarks) {
  for (int i = 0; i < num_landmarks; i++) {
    landmarks_.push_back(road_network_.get_rand_node().id_);
  }
}
