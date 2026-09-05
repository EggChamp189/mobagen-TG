#include "SeparationRule.h"
#include "imgui.h"
#include <glm/glm.hpp>

glm::vec2 SeparationRule::computeForce(const std::vector<BoidView>& neighborhood, const BoidView& boid) {
  glm::vec2 separatingForce(0.f);

  // the header have the desiredMinimalDistance member variable, which is the distance that the boids should try to maintain from each other.
  // glm::length(vec) returns the length of a vector,
  // glm::normalize(vec) returns the normalized vector (length 1) in the same direction as vec.
  // multiply by (desiredMinimalDistance / distance) is the proportionality factor that makes the force stronger when the boids are closer together, and weaker when they are farther apart.

  // begin solution


  glm::vec2 direction;
  float distance;

  // calculate the inverse force and add it to the separating force, then average 
  for (BoidView other : neighborhood) {
    direction = boid.position - other.position; // this gets the direction from this boid pointing to the other boid
    distance = length(direction);
    if (distance < 0.0001f) { // prevent outright diving by anything close to 0
      distance = 0.0001f;
    }
    separatingForce += normalize(direction) * (desiredMinimalDistance / distance);  // the closer the birds are, the faster they should move away.
  }

  // end solution

  return separatingForce ;
}

bool SeparationRule::drawImguiRuleExtra() {
  bool valueHasChanged = false;
  if (ImGui::DragFloat("Desired Separation", &desiredMinimalDistance, 0.05f)) {
    valueHasChanged = true;
  }
  return valueHasChanged;
}
