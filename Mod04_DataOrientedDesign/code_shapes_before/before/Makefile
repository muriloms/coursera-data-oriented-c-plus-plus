demo: demo.o square.o circle.o
	$(CXX) -o $@ $^

demo.o: demo.cpp square.h circle.h shape.h color.h
	$(CXX) -c -o $@ $<

square.o: square.cpp square.h shape.h
	$(CXX) -c -o $@ $<

circle.o: circle.cpp circle.h shape.h
	$(CXX) --std=c++23 -c -o $@ $<

clean:
	rm *.o
