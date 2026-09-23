#include "HuntAndKillExample.h"
#include "../World.h"
#include "../SeededRandom.h"
#include "Random.h"
#include <climits>
bool HuntAndKillExample::Step(World* w) {
  // todo: code this
  // the algorithm works by starting out in a killing phase, where it chooses a random starting point, 
  // randomly walks around, then removes walls from one of it's unvisited neighbors similar to regular recursive.
  // once it gets stuck though, it then transitions into hunt mode, where it scans the entire grid line by line 
  // until it finds an unvisited spot, then continues the killing phase from there. 
  // this repeats until it cannot find a single unvisited space.

  // if the last hunt resulted in no spots found, then end.
  if (currentSpot.x == INT_MAX && currentSpot.y == INT_MAX) return false;

  // begin with getting visitables, as will always be helpful
  std::vector<Point2D> curVisitables = getVisitables(w, currentSpot);

  // Start by checking if our spot surrounded by filled spots, and if so, start hunting left and wrapping
  // if this is the case, the last loop should have already marked this spot as visited so I shouldn't need to.
  if (curVisitables.size() <= 0 && isKillingNotHunting) {
    currentSpot = randomStartPoint(w); // then go to the next available spot if possible
    isKillingNotHunting = false; // switch to hunting
    return true;
  }

  // if we have already been visited
  // temp variable if found a new spot to move, don't waste a random slot if only 1 unvisited spot returned
  Point2D newSpot;
  if (curVisitables.size() >= 2)
    newSpot = curVisitables[SeededRandom::next() % curVisitables.size()];
  else
    newSpot = curVisitables[0];
  // the new spot will always become visited
  visited[newSpot.y][newSpot.x] = true;
  // if you were hunting, then choose a path from the visited around and switch to killing
  if (isKillingNotHunting) {
    std::vector<Point2D> curVisited = getVisitedNeighbors(w, newSpot); // get the visited neighbors around the new spot
    Point2D newUnvisited;                                               // need an extra point to switch easier
    if (curVisited.size() >= 2)
      newUnvisited = curVisited[SeededRandom::next() % curVisited.size()];
    else
      newUnvisited = curVisited[0];
    // change the current spot to be the randomly selected visited one in order to allow the wall destroying logic to still work.
    currentSpot = newUnvisited;
    // switch to the killing once complete
    isKillingNotHunting = true;
  }

  Point2D worldCur = w->ToWorldCoords(currentSpot);
  w->SetNodeColor(w->ToWorldCoords(currentSpot), Color32(1.0f, 0.0f, 0.0f, 1.0f));

  // set the wall on the new node
  if (newSpot.y < currentSpot.y) w->SetNorth(worldCur, false); // if the new position's (stack.back) y is less than the old one, we moved up one (starting at the current to the new)
  else if (newSpot.x > currentSpot.x) w->SetEast(worldCur, false);  // if the new position's (stack.back) x is more than the old one, we moved right one
  else if (newSpot.y > currentSpot.y) w->SetSouth(worldCur, false);  // if the new position's (stack.back) y is more than the old one, we moved down one
  else if (newSpot.x < currentSpot.x) w->SetWest(worldCur, false); // if the new position's (stack.back) x is less than the old one, we moved left one
  currentSpot = newSpot; // switch for the next loop

  return true;
}
void HuntAndKillExample::Clear(World* world) {
  visited.clear();
  //stack.clear(); // unneeded I believe

  // this generates the grid so that the x is the sideways row, and y is the upwards row. 
  // it also means x and y are reversed in the sense of regular coordinate calculation, so (0, 1) means the second column on the first row, eg:
  // [ ][o]             [ ][ ]
  // [ ][ ] instead of  [o][ ]
  // the world coordinates hosts them the opposite way though, so when sending in to color or set a wall use regular coords {x, y} instead of {y, x} for accessing a point
  for (int i = 0; i < world->GetHeight(); i++) {
    for (int j = 0; j < world->GetWidth(); j++) {
      visited[i][j] = false;
    }
  }
  //visited[0][0] = true;
  //visited[1][0] = true; // so the layout here is [y][x]
  //world->SetNodeColor(world->ToWorldCoords({0,0}), Color32(0.0f, 1.0f, 0.0f, 1.0f));
  //world->SetNodeColor(world->ToWorldCoords({1,0}), Color32(0.0f, 1.0f, 0.0f, 1.0f));
  currentSpot = randomStartPoint(world); // set a new starting point
  std::cout << "Current Starting spot: " << currentSpot.x << ", " << currentSpot.y << "\n";
  visited[currentSpot.y][currentSpot.x] = true; // may need to be reversed?
  //world->SetNodeColor(world->ToWorldCoords(currentSpot), Color32(1.0f, 0.0f, 0.0f, 1.0f));
}

// this doesn't actually find a random one, just 
Point2D HuntAndKillExample::randomStartPoint(World* world) {
  // Todo: improve this if you want
  for (int y = 0; y < world->GetHeight(); y++)
    for (int x = 0; x < world->GetWidth(); x++) {
      //world->SetNodeColor(world->ToWorldCoords({x, y}), Color32(0.0f, 1.0f, 0.0f, 1.0f)); //this is correct I think
      if (!visited[y][x]) return {x, y};
    }
  return {INT_MAX, INT_MAX};
}

// for both of these, I just copied over the method I used in the recursive backtrack and made relevant adjustments to the logic and coordinates

// specifically checks for unvisited neighbors to move to in kill mode and when searching for an unvisited neighbor to create a new path off of in hunt mode.
std::vector<Point2D> HuntAndKillExample::getVisitables(World* w, const Point2D& p) {
  int x = p.x;
  int y = p.y;
  std::vector<Point2D> points;
  std::vector<Point2D> directions = {{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};

  // first check the original point is within the bounds
  if (0 <= x && x < w->GetWidth() && 0 <= y && y < w->GetHeight()) {
    // loop through the directions
    for (int i = 0; i < 4; i++) {
      // check if the new point with the added direction is within bounds
      if (0 <= directions[i].x && directions[i].x < w->GetWidth() && 0 <= directions[i].y
          && directions[i].y < w->GetHeight()) {  // this may be off by one, may need to subtract
        // check if the point isn't already visited
        if (!visited[directions[i].y][directions[i].x]) {
          points.push_back(directions[i]);  // send it if it isn't marked as visited
        }
      }
    }
  }
  // end solution
  return points;
}

// specifically checks for visited neighbors in hunt mode to continue a path off of when an unvisited neighbor is found.
std::vector<Point2D> HuntAndKillExample::getVisitedNeighbors(World* w, const Point2D& p) {
  //std::vector<Point2D> deltas = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
  int x = p.x;
  int y = p.y;
  std::vector<Point2D> points;
  std::vector<Point2D> directions = {{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};

  // first check the original point is within the bounds
  if (0 <= x && x < w->GetWidth() && 0 <= y && y < w->GetHeight()) {
    // loop through the directions
    for (int i = 0; i < 4; i++) {
      // check if the new point with the added direction is within bounds
      if (0 <= directions[i].x && directions[i].x < w->GetWidth() && 0 <= directions[i].y
          && directions[i].y < w->GetHeight()) {  // this may be off by one, may need to subtract
        // check if the point is already visited
        if (visited[directions[i].y][directions[i].x]) {
          points.push_back(directions[i]);  // send it if it is marked as visited
        }
      }
    }
  }
  // end solution
  return points;
}
