#include "../World.h"
#include "../SeededRandom.h"
#include "RecursiveBacktrackerExample.h"
#include <climits>

// Recursive backtracker, in FORMAL units: (0, 0) is the top-left cell, x grows
// right, y grows down. The caller seeds SeededRandom before the first Step;
// every decision consumes the seed in order, so the maze is deterministic.
//
// Procedure per Step, on the cell at the top of the path stack:
//   1. mark it visited;
//   2. list its visitable (unvisited) neighbors in clockwise order starting
//      from the top: UP, RIGHT, DOWN, LEFT (getVisitables does this);
//   3. none        -> dead end: pop the stack (backtrack). Empty stack = done;
//   4. exactly one -> move to it, do not consume a random number;
//   5. two or more -> consume SeededRandom::next() and pick
//      next() % visitableCount;
//   6. moving opens the wall between the two cells
//      (World::SetNorth/SetEast/SetSouth/SetWest with false).

void RecursiveBacktrackerExample::Clear(World* world) {
  // todo: reset the walk
  // hint:
  //   clear visited and the path stack, then start the walk at the
  //   top-left cell in formal units: stack.push_back({0, 0})
  // begin solution
  
  // stack should clear fine
  stack.clear();

  // the way visited works is that the first int in the map is the x coordinate, while the second is another map. that other map's first variable is the y, 
  // and the second variable is the bool that determines if the spot has already been visited
  // visited[1][1] = true; // this would make the coordinate at x = 1 and y = 1 already visited.
  // clear each visited's sub maps first
  for (auto v : visited) {
    v.second.clear(); // may need to do something with first since that appears to be a pair too
  }

  // once the inside maps are cleared it can be safely cleared
  visited.clear(); 

  // get the world size in formal coords
  Point2D size = {world->GetWidth(), world->GetHeight()}; 
  // refresh the entire grid by placing an unvisited spot in every formal coordinate
  for (int x = 0; x < size.x; x++) {
    std::map<int, bool> visitedNew;
    for (int y = 0; y < size.y; y++) {
      visitedNew.insert({y, false});
    }
    visited.insert({x, visitedNew});
  }
  // push a new stack spot back
  stack.push_back({0, 0});
  // end solution
}

bool RecursiveBacktrackerExample::Step(World* w) {
  // todo: implement one iteration of the recursive backtracker
  // hint:
  //   empty stack  -> the maze is done, return false
  //   otherwise, on the cell at the top of the stack (formal units):
  //   1. mark it visited;
  //   2. list its visitable neighbors with getVisitables
  //      (already in clockwise order: UP, RIGHT, DOWN, LEFT);
  //   3. none        -> dead end: pop the stack (backtrack);
  //   4. exactly one -> move to it, do not consume a random number;
  //   5. two or more -> consume SeededRandom::next() and pick
  //      next() % visitables.size();
  //   moving = opening the wall between the two cells, through the
  //   World coordinate translation:
  //     Point2D worldCurrent = w->ToWorldCoords(current);
  //     UP    -> w->SetNorth(worldCurrent, false) // use these for changing the wall on the named orientation side on or off
  //     RIGHT -> w->SetEast(worldCurrent, false)
  //     DOWN  -> w->SetSouth(worldCurrent, false)
  //     LEFT  -> w->SetWest(worldCurrent, false)
  //   return true while there is still work (stack not empty after the move)
  // begin solution
  if (stack.empty()) return false; // stack is empty therefore done

  Point2D current = stack.back();   // keep for determining what walls remain laters
  std::vector<Point2D> curVisitables = getVisitables(w, stack.back());  // get all the relevant directions

  // first check if further backtracking should happen by checking:
  // 1: if this node is already marked as visited, and
  // 2: checking that no more neightbors to exist to try a different route
  if (visited[current.x][current.y] && curVisitables.size() == 0) {
    // mark this node as red to show it can still function on backtrack to the debuggers
    w->SetNodeColor(w->ToWorldCoords(current), Color32(0.0f, 0.0f, 0.0f, 1.0f));  // should set to black when continuing to backtrack from a dead end
    stack.pop_back();
    return true;
  }

  visited[current.x][current.y] = true;  // mark this current node as visited so it doesn't accidentally backtrack over itself

  // check if backtracking should actively begin
  if (curVisitables.size() == 0) {
    w->SetNodeColor(w->ToWorldCoords(current), Color32(0.0f, 0.0f, 0.0f, 1.0f));  // should set to black when a dead end is hit
    stack.pop_back();
    return true; // we instead backtrack rather than use the regular code
  }  
  // next if there is just one visitable, return the only one given instead of wasting a random slot.
  else if (curVisitables.size() == 1) {
    w->SetNodeColor(w->ToWorldCoords(current), Color32(1.0f, 0.0f, 0.0f, 1.0f));  // should set the current node to red
    stack.push_back(curVisitables[0]);
  } 
  else {
    w->SetNodeColor(w->ToWorldCoords(current), Color32(1.0f, 0.0f, 0.0f, 1.0f));  // should set the current node to red
    stack.push_back(curVisitables[SeededRandom::next() % curVisitables.size()]); // choose a random direction and push that back
  }

  // anything past this point assumes that one visitable neighbor was pushed back and is the new back in the stack

  // set the new point node to green to show that the spot between the red and green is where the wall should be removed
  w->SetNodeColor(w->ToWorldCoords(stack.back()), Color32(0.0f, 1.0f, 0.0f, 1.0f));
  Point2D worldCur = w->ToWorldCoords(current); // gets the world position of the current node
  // remove the wall between the space we were (current) and just moved to (stack.back)
  if (stack.back().y < current.y) w->SetNorth(worldCur, false); // if the new position's (stack.back) y is less than the old one, we moved up one (starting at the current to the new)
  else if (stack.back().x > current.x) w->SetEast(worldCur, false);  // if the new position's (stack.back) x is more than the old one, we moved right one
  else if (stack.back().y > current.y) w->SetSouth(worldCur, false);  // if the new position's (stack.back) y is more than the old one, we moved down one
  else if (stack.back().x < current.x) w->SetWest(worldCur, false); // if the new position's (stack.back) x is less than the old one, we moved left one
  // end solution
  return true;
}

std::vector<Point2D> RecursiveBacktrackerExample::getVisitables(World* w, const Point2D& formalPoint) {
  // todo: list the unvisited neighbors of formalPoint, in clockwise order
  // hint:
  //   candidates in order: UP {x, y-1}, RIGHT {x+1, y}, DOWN {x, y+1}, LEFT {x-1, y}
  //   keep a candidate only if it is inside the grid
  //   (0 <= x < w->GetWidth(), 0 <= y < w->GetHeight()) and not visited
  // begin solution
  int x = formalPoint.x;
  int y = formalPoint.y;
  std::vector<Point2D> points;
  std::vector<Point2D> directions = {{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};

  // first check the original point is within the bounds
  if (0 <= x < w->GetWidth() && 0 <= y < w->GetHeight()) {
    // loop through the directions
    for (int i = 0; i < 4; i++) {
      // check if the new point with the added direction is within bounds
      if (0 <= directions[i].x && directions[i].x < w->GetWidth() && 
          0 <= directions[i].y && directions[i].y < w->GetHeight()) {  // this may be off by one, may need to subtract
        // check if the point isn't already visited
        if (!visited[directions[i].x][directions[i].y]) {
          points.push_back(directions[i]); // send it if it isn't marked as visited
        }
      }
    }
  }
  // end solution
  return points;
}
