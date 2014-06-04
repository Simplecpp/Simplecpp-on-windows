#include <simplecpp>
#include <graphics.h>
Turtle::Turtle(Composite* owner){
  double pts_body[4][2] = {{0, 15}, {20, 0}, {0, -15}, {0,15}};
  //double pts_body[4][2] = {{0, 15}, {20, 0}, {0, 150}, {0,15}};
  //setColor(COLOR("red"), false);

  setColor(COLOR(255,0,0), false);
  setFill();
  penDown();
  reset(canvas_width()/2, canvas_height()/2,pts_body, 4, owner);
}
