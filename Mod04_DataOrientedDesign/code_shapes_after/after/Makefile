demo: demo.o shape.o
	$(CXX) -o $@ $^

demo.o: demo.cpp shape.h color.h
	$(CXX) -c -o $@ $<

shape.o: shape.cpp shape.h
	$(CXX) --std=c++23 -c -o $@ $<

clean:
	rm *.o
