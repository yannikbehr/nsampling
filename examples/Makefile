FLAGS=-std=c++11 -g

ns: ns.cpp ../src/nested_sampling.cpp ../src/distributions.cpp
	g++ ns.cpp -o ns ${FLAGS} -I../src ../src/nested_sampling.cpp ../src/distributions.cpp

clean:
	-rm ns