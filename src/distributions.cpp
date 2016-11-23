#include <distributions.h>
#include <cstdlib>
#include <cmath>
#include <random>

std::random_device Uniform::r;
std::default_random_engine Uniform::e = std::default_random_engine(Uniform::r());

double Uniform::draw(){
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	u = uniform_dist(e);
	return (xmax-xmin)*u + xmin;
}

double Uniform::draw(double step){
	std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);
	u += step * uniform_dist(e);
	u -= floor(u); // wraparound to stay within (0,1)
	return (xmax-xmin)*u + xmin;
}

Uniform::Uniform(std::string name, double min, double max){
		inst_name = name;
		xmin = min;
		xmax = max;
		u = 0.;
}

Uniform::Uniform(const Uniform& other){
	inst_name = other.inst_name;
	xmin = other.xmin;
	xmax = other.xmax;
	u = other.u;
}

Uniform* Uniform::clone(){
	return new Uniform(*this);
}

double Uniform::get_value(){
	return (xmax-xmin)*u + xmin;
}

void Uniform::set_value(double value){
	x = value;
}

std::string Uniform::get_name(){
	return inst_name;
}




double CUniform::draw(){
	u = (rand()+0.5)/(RAND_MAX+1.0);
	return (xmax-xmin)*u + xmin;
}

double CUniform::draw(double step){
	u += step * (2.*(rand()+0.5)/(RAND_MAX+1.0) -1.);
	u -= floor(u); // wraparound to stay within (0,1)
	return (xmax-xmin)*u + xmin;
}

CUniform::CUniform(std::string name, double min, double max){
		inst_name = name;
		xmin = min;
		xmax = max;
		u = 0.;
}

CUniform::CUniform(const CUniform& other){
	inst_name = other.inst_name;
	xmin = other.xmin;
	xmax = other.xmax;
	u = other.u;
}

CUniform* CUniform::clone(){
	return new CUniform(*this);
}

double CUniform::get_value(){
	return (xmax-xmin)*u + xmin;
}

void CUniform::set_value(double value){
	x = value;
}

std::string CUniform::get_name(){
	return inst_name;
}
