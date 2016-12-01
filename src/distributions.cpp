#include <distributions.h>
#include <cstdlib>
#include <cmath>
#include <random>

std::random_device Uniform::_r;
std::default_random_engine Uniform::_e = std::default_random_engine(Uniform::_r());

double Uniform::draw(){
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	_u = uniform_dist(_e);
	return (_xmax-_xmin)*_u + _xmin;
}

double Uniform::draw(double step){
	std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);
	_u += step * uniform_dist(_e);
	_u -= floor(_u); // wraparound to stay within (0,1)
	return (_xmax-_xmin)*_u + _xmin;
}

Uniform::Uniform(std::string name, double min, double max){
		_inst_name = name;
		_xmin = min;
		_xmax = max;
		_u = 0.;
}

Uniform::Uniform(const Uniform& other){
	_inst_name = other._inst_name;
	_xmin = other._xmin;
	_xmax = other._xmax;
	_u = other._u;
}

Uniform* Uniform::clone(){
	return new Uniform(*this);
}

double Uniform::get_value(){
	return (_xmax-_xmin)*_u + _xmin;
}

std::string Uniform::get_name(){
	return _inst_name;
}




double CUniform::draw(){
	_u = (rand()+0.5)/(RAND_MAX+1.0);
	return (_xmax-_xmin)*_u + _xmin;
}

double CUniform::draw(double step){
	_u += step * (2.*(rand()+0.5)/(RAND_MAX+1.0) -1.);
	_u -= floor(_u); // wraparound to stay within (0,1)
	return (_xmax-_xmin)*_u + _xmin;
}

CUniform::CUniform(std::string name, double min, double max){
		_inst_name = name;
		_xmin = min;
		_xmax = max;
		_u = 0.;
}

CUniform::CUniform(const CUniform& other){
	_inst_name = other._inst_name;
	_xmin = other._xmin;
	_xmax = other._xmax;
	_u = other._u;
}

CUniform* CUniform::clone(){
	return new CUniform(*this);
}

double CUniform::get_value(){
	return (_xmax-_xmin)*_u + _xmin;
}

std::string CUniform::get_name(){
	return _inst_name;
}
