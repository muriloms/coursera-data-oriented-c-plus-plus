#include "color.h"
#include "shape.h"
#include <vector>
#include <iostream>

int main(void) {
  Color red = {255, 0, 0};
  Color purple = {128, 0, 128};
  std::vector<Shape> shapes;
  shapes.push_back( Shape(1, red, Kind::Square));
  shapes.push_back( Shape(0.25, purple, Kind::Square));
  shapes.push_back( Shape( 1.5, red, Kind::Circle ));
  shapes.push_back( Shape( 0.25, purple, Kind::Circle ));
  float total_perimeter {};
  float total_area {};

  for( Shape s: shapes ) {
    total_perimeter += s.perimeter();
    total_area += s.area();
  }

  std::cout << (total_area/total_perimeter) << std::endl;


}
