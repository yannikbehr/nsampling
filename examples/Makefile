CFLAGS=-std=c++11 -g

ns: ns.cpp
	g++ -I../src ns.cpp ../src/nested_sampling.cpp ../src/distributions.cpp -o ns $(CFLAGS) 
	
run:
	./ns

clean:
	-rm ns
