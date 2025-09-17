
#include "circle.h"
#include <numbers>


float Circle::area() {
  return radius * radius * std::numbers::pi;
}

float Circle::perimeter() {
  return 2*radius * std::numbers::pi; 
}
