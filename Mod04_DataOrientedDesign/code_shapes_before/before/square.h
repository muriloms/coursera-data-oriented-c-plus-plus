#ifndef _square_h
#define _square_h

#include "shape.h"
#include <memory>

struct Square: Shape {
  float side;
  Square( float _side, std::shared_ptr<Color> _color ): side(_side), Shape(_color) {}

  virtual float area();
  virtual float perimeter();
};

#endif //_square_h
