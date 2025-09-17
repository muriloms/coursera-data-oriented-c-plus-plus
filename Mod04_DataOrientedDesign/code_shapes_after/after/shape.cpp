#include "shape.h"

float Shape::area() {
  switch(kind) {
    case Kind::Square: 
      return 2*radius * 2*radius; 
    case Kind::Circle:
      return std::numbers::pi * radius*radius;
  }
  return 0.0; 
}
float Shape::perimeter() {
  switch(kind) {
    case Kind::Square: 
      return 4 * 2*radius; 
    case Kind::Circle:
      return std::numbers::pi * 2*radius;
  }
  return 0.0;
}
