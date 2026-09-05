#include "AlignmentRule.h"
#include <glm/glm.hpp>

glm::vec2 AlignmentRule::computeForce(const std::vector<BoidView>& neighborhood, const BoidView& boid) {
  glm::vec2 averageVelocity(0.f);
  // glm::vec2 can be divided by a float, which will divide each component of the vector by that float.

  // begin solution
  if (neighborhood.size() == 0) return averageVelocity;

  for (BoidView other : neighborhood) {
    averageVelocity += other.velocity;
  }
  averageVelocity /= neighborhood.size();
  //averageVelocity /= 4.0f; // so it doesn't turn as fast (looking nicer to me)
  // average the velocity between the new direction and the current one for even smoother turning
  //averageVelocity += boid.velocity;
  //averageVelocity /= 2.0f;
  
  // last nerf to make the overall force slower (All the customization and joy and whimsy was removed for tests :( )
  return averageVelocity;
  // end solution
}
