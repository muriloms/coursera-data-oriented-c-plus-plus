
#ifndef _circle_h
#define _circle_h

#include "shape.h"
#include <memory>


struct Circle: Shape {
  float radius;
  Circle( float _radius, std::shared_ptr<Color> _color ): radius(_radius), Shape(_color) {}

  virtual float area();
  virtual float perimeter();
};

#endif //_circle_h
