#include "color.h"
#include "shape.h"
#include "square.h"
#include "circle.h"
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

int main(void) {
  auto red = make_shared<Color>(Color {255, 0, 0} );
  auto purple = make_shared<Color>(Color {128, 0, 128 });
  std::vector<shared_ptr<Shape>> shapes;
  shapes.push_back( make_shared<Square>(2, red));
  shapes.push_back( make_shared<Square>(0.5, purple));
  shapes.push_back( make_shared<Circle>( 1.5, red ));
  shapes.push_back( make_shared<Circle>( 0.25, purple ));
  float total_perimeter {};
  float total_area {};

  for( shared_ptr<Shape> s: shapes ) {
    total_perimeter += s->perimeter();
    total_area += s->area();
  }

  cout << (total_area/total_perimeter) << endl;


}
