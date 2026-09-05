#include "CohesionRule.h"
#include <glm/glm.hpp>

glm::vec2 CohesionRule::computeForce(const std::vector<BoidView>& neighborhood, const BoidView& boid) {
  glm::vec2 cohesionForce(0.f);

  // glm::length(vec) returns the length of a vector,
  // glm::normalize(vec) returns the normalized vector (length 1) in the same direction as vec.

  // begin solution

  if (neighborhood.size() == 0) return cohesionForce;

  glm::vec2 point(0.f);

  // calculate the average position to be in by averaging all the non main boid positions
  for (BoidView other : neighborhood) {
    point += other.position;
  }

  point /= neighborhood.size();
  // then calculate the force from the boid to that averaged position
  cohesionForce = point - boid.position;


  // end solution

  return normalize(cohesionForce);
}
