#ifndef _shape_h
#define _shape_h

#include "color.h"
#include <memory>


struct Shape {
  std::shared_ptr<Color> color;
  Shape( std::shared_ptr<Color> _color ): color(_color) {}

  virtual float area() = 0;
  virtual float perimeter() = 0;
};

#endif //_shape_h
