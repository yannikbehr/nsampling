#include "distributions.h"
#include <cstdlib>
#include <cmath>
#include <random>

std::random_device Uniform::_r;
std::default_random_engine Uniform::_e = std::default_random_engine(Uniform::_r());

std::random_device Normal::_r;
std::default_random_engine Normal::_e = std::default_random_engine(Normal::_r());

std::random_device InvCDF::_r;
std::default_random_engine InvCDF::_e = std::default_random_engine(InvCDF::_r());

double Uniform::draw(){
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	_u = uniform_dist(_e);
	return (_xmax-_xmin)*_u + _xmin;
}

double Uniform::trial(double step){
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

double CUniform::trial(double step){
	_u += step * (2.*(rand()+0.5)/(RAND_MAX+1.0) -1.);
	_u -= floor(_u); // wraparound to stay within (0,1)
	return (_xmax-_xmin)*_u + _xmin;
}

CUniform::CUniform(std::string name, double min, double max){
		_inst_name = name;
		_xmin = min;
		_xmax = max;
		_u = 0.;
		srand(42);
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


Normal::Normal(std::string name, double mean, double sigma){
	_inst_name = name;
	_mean = mean;
	_sigma = sigma;
	_u1 = 0.;
	_u2 = 0.;
	_y = 0.;
	_pi = 4*atan(1);
}

Normal::Normal(const Normal& other){
	_inst_name = other._inst_name;
	_mean = other._mean;
	_sigma = other._sigma;
	_u1 = other._u1;
	_u2 = other._u2;
	_y = other._y;
	_pi = 4*atan(1);
}

Normal* Normal::clone(){
	return new Normal(*this);
}

double Normal::draw(){
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	_u1 = uniform_dist(_e);
	_u2 = uniform_dist(_e);
	// Box-Muller method
	_y = sqrt(-2*log(_u1))*cos(2*_pi*_u2);
	return _sigma*_y+_mean; 
}

double Normal::trial(double step){
	std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);
	_u1 += step * uniform_dist(_e);
	_u2 += step * uniform_dist(_e);
	// wraparound to stay within (0,1)
	_u1 -= floor(_u1); 
	_u2 -= floor(_u2);
	_y = sqrt(-2*log(_u1))*cos(2*_pi*_u2);
	return _sigma*_y+_mean; 
}

double Normal::get_value(){
	return _sigma*_y + _mean;
}

std::string Normal::get_name(){
	return _inst_name;
}


InvCDF::InvCDF(std::string name, std::vector<double> x, std::vector<double> p, int seed){
	_inst_name = name;
	_x = x;
	_p = p;
	if(seed > 0)
		_e = std::default_random_engine(seed);
}

InvCDF::InvCDF(const InvCDF& other){
	_inst_name = other._inst_name;
	_x = other._x;
	_p = other._p;
	_u = other._u;
}

double InvCDF::draw(){
	int lo, mid, hi;
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
	_u = uniform_dist(_e);
	if(_u <= _p.front())
    		return _x.front();
	if(_u >= _p.back())
    		return _x.back();
	lo = 0;
	hi = _p.size();
	while(lo < hi){
    		mid = (int)(hi + lo)/2;
    		if(_u < _p[mid]){
        		hi = mid;
    		}else{
        		lo = mid + 1;
    		}	
	}
	_val = _x[lo-1];
	return _val;
}

double InvCDF::trial(double step){
	int lo, mid, hi;
	std::uniform_real_distribution<double> uniform_dist(-1.0, 1.0);
	_u += step * uniform_dist(_e);
	_u -= floor(_u); // wraparound to stay within (0,1)
	if(_u <= _p.front())
    		return _x.front();
	if(_u >= _p.back())
    		return _x.back();
	lo = 0;
	hi = _p.size();
	while(lo < hi){
    		mid = (int)(hi + lo)/2;
    		if(_u < _p[mid]){
        		hi = mid;
    		}else{
        		lo = mid + 1;
    		}	
	}
	_val = _x[lo-1];
	return _val;
}

double InvCDF::get_value(){
	return _val;
}

std::string InvCDF::get_name(){
	return _inst_name;
}

InvCDF* InvCDF::clone(){
	return new InvCDF(*this);
}
