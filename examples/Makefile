CFLAGS=-std=c++11 -g

ns: ns.cpp
	g++ ns.cpp -o ns $(CFLAGS) -l:libsampling.a
	
run:
	./ns

clean:
	-rm ns