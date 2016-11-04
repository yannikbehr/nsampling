#include <distributions.h>
#include <cstdlib>
#include <cmath>
#include <random>

std::default_random_engine Uniform::e;

double Uniform::draw(){
	// If it's the first time this is called chose an arbitrary point
	if(!initialized){
		std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
		u = uniform_dist(e);
		initialized = true;
	}else{
		// Seed with a real random value, if available
		std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);
		u += _step * uniform_dist(e);
		u -= floor(u); // wraparound to stay within (0,1)
	}
	return (xmax-xmin)*u + xmin;
}

Uniform::Uniform(std::string name, double min, double max, double step){
		inst_name = name;
		xmin = min;
		xmax = max;
		_step = step;
		u = 0.;
		initialized = false;
}

Uniform::Uniform(const Uniform& other){
	inst_name = other.inst_name;
	xmin = other.xmin;
	xmax = other.xmax;
	_step = other._step;
	u = other.u;
	initialized = other.initialized;
}

Uniform* Uniform::clone(){
	return new Uniform(*this);
}

double Uniform::get_value(){
	return (xmax-xmin)*u + xmin;
}

double Uniform::get_step(){
	return _step;
}

void Uniform::set_step(double new_step){
	_step = new_step;
}

void Uniform::set_value(double value){
	x = value;
}

std::string Uniform::get_name(){
	return inst_name;
}
