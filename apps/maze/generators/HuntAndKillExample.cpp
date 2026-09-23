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
  
  // protect against someone pressing step after it already ended. (I accidentally triggered an error without this since I used an autoclicker lol)
  if (currentSpot.x == INT_MAX) return false;

  // begin with getting visitables, as will always be helpful
  std::vector<Point2D> curVisitables = getVisitables(w, currentSpot);

  // all killing code should go in here
  if (isKillingNotHunting) {
    // Start by checking if our spot surrounded by filled spots, and if so, start hunting left and wrapping
    // if this is the case, the last loop should have already marked this spot as visited so I shouldn't need to.
    if (curVisitables.size() <= 0) {
      // color in the spot we just jumped from for consistency
      w->SetNodeColor(w->ToWorldCoords(currentSpot), Color32(1.0f, 0.0f, 0.0f, 1.0f)); 
      // check if there is a next spot
      currentSpot = findNewHuntSpot(w); 
      // if it wasn't possible, just quit
      if (currentSpot.x == INT_MAX) return false;

      // otherwise, use blue to show that the next spot started by hunting there.
      w->SetNodeColor(w->ToWorldCoords(currentSpot), Color32(0.0f, 0.0f, 1.0f, 1.0f));
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

    Point2D worldCur = w->ToWorldCoords(currentSpot);
    w->SetNodeColor(w->ToWorldCoords(currentSpot), Color32(1.0f, 0.0f, 0.0f, 1.0f));
    w->SetNodeColor(w->ToWorldCoords(newSpot), Color32(0.0f, 1.0f, 0.0f, 1.0f));

    // set the wall on the new node
    if (newSpot.y < currentSpot.y) w->SetNorth(worldCur, false); // if the new position's (stack.back) y is less than the old one, we moved up one (starting at the current to the new)
    else if (newSpot.x > currentSpot.x) w->SetEast(worldCur, false);  // if the new position's (stack.back) x is more than the old one, we moved right one
    else if (newSpot.y > currentSpot.y) w->SetSouth(worldCur, false);  // if the new position's (stack.back) y is more than the old one, we moved down one
    else if (newSpot.x < currentSpot.x) w->SetWest(worldCur, false); // if the new position's (stack.back) x is less than the old one, we moved left one
    
    currentSpot = newSpot; // switch for the next loop

    return true;
  }

  // hunting otherwise
  // use the last spot found when we switched to hunting and name it hunt spot for ease of reading
  Point2D nextHuntSpot = currentSpot;

  // if the last hunt resulted in no spots found, then end if it wasn't caught before
  // we don't need to do this actually since we already check before when we calculated the last currentSpot
  //if (nextHuntSpot.x == INT_MAX) return false;

  // if you were hunting, then choose a path from the visited around it and switch to killing
  std::vector<Point2D> curVisited = getVisitedNeighbors(w, nextHuntSpot);  // get the visited neighbors around the new spot

  assert(curVisited.size() != 0); // when hunting, you should always be next to a visited spot or the core idea failed

  Point2D newVisited;  // need an extra point to switch easier

  // choose a new visited point
  if (curVisited.size() >= 2)
    newVisited = curVisited[SeededRandom::next() % curVisited.size()];
  else
    newVisited = curVisited[0];

  // translate the new visited to world coordinates, as it will be the point we create a wall from. 
  // We could reverse it and have nextHuntSpot be the same, but the copied and pasted logic from the hunt code works with nextHuntSpot being the one checked from 
  Point2D worldCur = w->ToWorldCoords(newVisited);
  w->SetNodeColor(w->ToWorldCoords(nextHuntSpot), Color32(1.0f, 0.0f, 0.0f, 1.0f));

    // set the wall on the new node
    if (nextHuntSpot.y < newVisited.y) w->SetNorth(worldCur, false); // if the new position's (nextHuntSpot) y is less than the newVisited, we moved up one (starting at the current to the new)
    else if (nextHuntSpot.x > newVisited.x) w->SetEast(worldCur, false);  // if the new position's (nextHuntSpot) x is more than the newVisited, we moved right one
    else if (nextHuntSpot.y > newVisited.y) w->SetSouth(worldCur, false);  // if the new position's (nextHuntSpot) y is more than the newVisited, we moved down one
    else if (nextHuntSpot.x < newVisited.x) w->SetWest(worldCur, false); // if the new position's (nextHuntSpot) x is less than the newVisited, we moved left one

  // Set the next hunting spot as visited. currentPoint was already set to nextHuntSpot, I just used nextHuntSpot while I thought I had to switch between multiple.
  visited[nextHuntSpot.y][nextHuntSpot.x] = true;

  // switch to the killing once complete
  isKillingNotHunting = true;

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
  for (int y = 0; y < world->GetHeight(); y++) {
    for (int x = 0; x < world->GetWidth(); x++) {
      visited[y][x] = false;
    }
  }

  // get an actual random spot on the map to start on
  currentSpot = {
      SeededRandom::next() % world->GetWidth(), 
      SeededRandom::next() % world->GetHeight()
  };

  // the current starting spot is said out loud for my sake of mind
  //std::cout << "Current Starting spot: " << currentSpot.x << ", " << currentSpot.y << "\n";
  // we mark the first node as visited, similar to pushing 0,0 in backtracker
  visited[currentSpot.y][currentSpot.x] = true; 
}

// this doesn't actually find a random one, just the next available from a sequential line
Point2D HuntAndKillExample::findNewHuntSpot(World* world) {
  // Todo: improve this if you want // I did :)
  for (int y = 0; y < world->GetHeight(); y++)
    for (int x = 0; x < world->GetWidth(); x++) {
      // instead of checking for visited being unvisited, we will ignore it if it is true and do additional code if true
      if (visited[y][x]) continue;

      // hunt for unvisited cells that also have a visited cell next to it
      std::vector<Point2D> nearbyVisited = getVisitedNeighbors(world, {x, y});

      // if we found something in the visited, we hunt here
      if (nearbyVisited.size() > 0) return {x, y};
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
