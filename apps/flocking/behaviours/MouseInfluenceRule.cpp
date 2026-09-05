#include "MouseInfluenceRule.h"
#include "imgui.h"

glm::vec2 MouseInfluenceRule::computeForce(const std::vector<BoidView>& neighborhood, const BoidView& boid) {
  glm::vec2 force(0.f);

  // ImGui::IsMouseDown(ImGuiMouseButton_Left) returns true if the left mouse button is currently pressed.
  // ImGui::GetIO().MousePos returns the current mouse position as an ImVec2.
  // glm::length(vec) returns the length of a vector

  // begin solution
  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) return force;

  glm::vec2 direction(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
  float distance;

  // calculate the inverse force and add it to the separating force, then average
  
  if (isRepulsive) direction = boid.position - direction;  // this gets the direction from this boid pointing to the mouse position
  else direction = direction - boid.position;  // this gets the direction from the mouse position pointing to this boid

  distance = length(direction);

  if (distance < 0.0001f) {  // prevent outright diving by anything close to 0
    distance = 0.0001f;
  }
  force = normalize(direction) * (10000.0f / distance);  // the closer the birds are, the faster they should move away.
  // end solution

  return force;
}

bool MouseInfluenceRule::drawImguiRuleExtra() {
  bool valueHasChanged = false;

  if (ImGui::RadioButton("Attractive", !isRepulsive)) {
    isRepulsive = false;
    valueHasChanged = true;
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("Repulsive", isRepulsive)) {
    isRepulsive = true;
    valueHasChanged = true;
  }

  return valueHasChanged;
}
