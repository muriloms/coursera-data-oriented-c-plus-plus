#ifndef _shape_h
#define _shape_h

#include <numbers>
#include "color.h"

enum class Kind {
  Square, Circle
};

struct Shape {
  Color color;
  Kind kind;
  float radius;

  Shape( float _radius, Color &_color, Kind _kind): color(_color), kind(_kind), radius(_radius) {}

  float area();
  float perimeter();
};

#endif //_shape_h
