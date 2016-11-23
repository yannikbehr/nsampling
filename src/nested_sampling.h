#ifndef NESTEDSAMPLING_H
#define NESTEDSAMPLING_H

#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <vector>
#include <distributions.h>

#define PLUS(x,y) (x > y ? x + log(1+exp(y-x)) : y + log(1+exp(x-y)))

class Callback {
public:
        virtual ~Callback() {}
        virtual double run(std::vector<double> vals) { std::cout << "Callback::run()" << std::endl; }
};

class Object{
public:
	double logL;
	double logWt;
	std::vector<Variable*> _vars;

	Object(std::vector<Variable*> vars);
	Object(Object& other);
	Object& operator=(const Object& other);
	std::vector<double> draw();
	std::vector<double> draw(double step);
	friend std::ostream& operator<<(std::ostream& os, const Object& o);

};


class Result{
public:
	std::vector<Object*> _samples;
	double _logZ, _H;
	int _n, _nvars;
	std::vector<double> _e, _var, _mx;
	std::vector<std::string> _vnames;

	Result(std::vector<Object*> Samples, double LogZ, double H, int n);
	~Result(){};
	void summarize();
	std::vector<double> getexpt(){return _e;};
	std::vector<double> getvar(){return _var;};
	std::vector<double> getmax(){return _mx;};
	std::vector<std::string> getnames(){return _vnames;};
	std::vector<double> getZ(){
		std::vector<double> vals;
		vals.push_back(_logZ);
		vals.push_back(std::sqrt(_H/_n));
		return vals;};
	double getH(){return _H;};

};


class NestedSampling{
private:
        Callback *_callback;
        std::vector<Variable*> _vars;
public:
	NestedSampling(std::vector<Variable*> vars): _callback(0){
		_vars = vars;
	}
	~NestedSampling() { delCallback(); }
	void delCallback() { delete _callback; _callback = 0; }
	void setCallback(Callback *cb) { delCallback(); _callback = cb; }
	void new_sample(Object *Obj, double logLstar);
	Result* explore(std::vector<Variable*> vars, int initial_samples,
			int maximum_steps);
};


#endif
