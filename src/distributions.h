#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <string>
#include <random>

class Variable{
public:
	virtual ~Variable(){};
	// Draw a new sample from the distribution
	virtual double draw() = 0;

	// Draw a sample around the previous sample using 'step' as the scaling
	// factor
	virtual double trial(double step) = 0;

	// Return the latest sample
	virtual double get_value() = 0;

	// Get the name of the random variable
	virtual std::string get_name() = 0;

	// Clone the random variable
	virtual Variable* clone() = 0;
};

/*
 * Uniform distribution
 */
class Uniform: public Variable{
private:
	double _u;
	double _xmin, _xmax;
	std::string _inst_name;

public:
	static std::random_device _r;
	static std::default_random_engine _e;
	double draw();
	double trial(double step);
	double get_value();
	std::string get_name();
	Uniform(std::string name, double min, double max,
		int seed=-1);
	Uniform(const Uniform& other);
	Uniform* clone();
};


/*
 * This is a uniform distribution using C's rand() to produce pseudo-random
 * numbers. It is only meant for unit testing and shouldn't be used for any
 * real applications due to known problems with rand().
 */
class CUniform: public Variable{
private:
	double _u;
	double _xmin, _xmax;
	std::string _inst_name;


public:
	double draw();
	double trial(double step);
	double get_value();
	std::string get_name();
	CUniform(std::string name, double min, double max);
	CUniform(const CUniform& other);
	CUniform* clone();
};


/*
 * Normal (Gaussian) distribution
 */
class Normal: public Variable{
private:
	double _u1, _u2, _y, _pi;
	double _mean, _sigma;
	std::string _inst_name;

public:
	static std::random_device _r;
	static std::default_random_engine _e;
	double draw();
	double trial(double step);
	double get_value();
	std::string get_name();
	Normal(std::string name, double mean,
	       	double sigma, int seed=-1);
	Normal(const Normal& other);
	Normal* clone();
};

/*
 * Constant
 */

class Constant: public Variable{
private:
	double _value;
	std::string _inst_name;

public:
	double draw(){ return _value;};
	double trial(double step){ return _value;};
	double get_value(){return _value;};
	std::string get_name(){ return _inst_name;};
	Constant(std::string name, double value){
		_inst_name = name;
		_value = value;
	};
	Constant( const Constant& other){
		_inst_name = other._inst_name;
		_value = other._value;
	};

	Constant* clone(){ return new Constant(*this);};
};

/*
 * Inverse Transform Sampling
 *
 * Sample from a given discrete CDF
 */

class InvCDF: public Variable{
private:
	double _u, _val;
	std::vector<double> _x, _p;
	std::string _inst_name;

public:
	static std::random_device _r;
	static std::default_random_engine _e;
	double draw();
	double trial(double step);
	double get_value();
	std::string get_name();
	InvCDF(std::string name, std::vector<double> x,
	       std::vector<double> p, int seed=-1);
	InvCDF(const InvCDF& other);
	InvCDF* clone();
};
#endif
