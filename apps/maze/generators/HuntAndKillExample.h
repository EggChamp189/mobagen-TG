#ifndef HUNTANDKILLEXAMPLE_H
#define HUNTANDKILLEXAMPLE_H

#include "../MazeGeneratorBase.h"
#include <string>
#include "math/Point2D.h"
#include <map>
#include <vector>

class HuntAndKillExample : public MazeGeneratorBase {
private:
  // std::vector<Point2D> stack; // stack is useless and irrelevant to hunt and kill
  Point2D currentSpot; // we only need one spot to use throughout the memory
  bool isKillingNotHunting = true; // if it is true, it is killing. if it is not, it is hunting.
  std::map<int, std::map<int, bool>> visited;  // naive. not optimal
  Point2D findNewHuntSpot(World* world); // I removed the starting point choice algorithm since it is easier to just generate in the clear function with Random % width and height
  std::vector<Point2D> getVisitables(World* w, const Point2D& p);
  std::vector<Point2D> getVisitedNeighbors(World* w, const Point2D& p);


public:
  HuntAndKillExample() = default;
  std::string GetName() override { return "HuntAndKill"; };
  bool Step(World* world) override;
  void Clear(World* world) override;
};

#endif  // HUNTANDKILLEXAMPLE_H
