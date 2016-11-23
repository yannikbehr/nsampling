#include "../src/nested_sampling.h"
#include "../src/distributions.h"
#include <iostream>

#define PI 3.1416

class LightHouse: public Callback{
public:
	double run(std::vector<double> vals){
		double x = vals[0];
		double y = vals[1];
		int N = 64;
		double D[64] = { 4.73, 0.45, -1.73, 1.09, 2.19, 0.12,
				1.31, 1.00, 1.32, 1.07, 0.86, -0.49, -2.59, 1.73, 2.11,
				1.61, 4.98, 1.71, 2.23, -57.20, 0.96, 1.25, -1.56, 2.45,
				1.19, 2.17, -10.66, 1.91, -4.16, 1.92, 0.10, 1.98, -2.51,
				5.55, -0.47, 1.91, 0.95, -0.78, -0.84, 1.72, -0.01, 1.48,
				2.70, 1.21, 4.41, -4.79, 1.33, 0.81, 0.20, 1.58, 1.29,
				16.19, 2.75, -2.38, -1.79, 6.50, -18.53, 0.72, 0.94, 3.64,
				1.94, -0.11, 1.57, 0.57};
		int k;
		double logL = 0;
		for(k=0; k<N; k++)
			logL += std::log((y/PI)/((D[k] - x)*(D[k] - x) + y*y));
		return logL;
	}
};

int main(){
	std::vector<Variable*> vars;
	vars.push_back(new Uniform("x", -2., 2.));
    vars.push_back(new Uniform("y", 0., 2.));
	NestedSampling ns;
	ns.setCallback(new LightHouse());
	Result *rs = ns.explore(vars,100,1000);
	rs->summarize();
	return 0;
}


