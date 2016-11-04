#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <string>
#include <random>

class Variable{
public:
	virtual double draw() = 0;
	virtual double get_value() = 0;
	virtual void set_value(double value) = 0;
	virtual void set_step(double new_step) = 0;
	virtual double get_step() = 0;
	virtual std::string get_name() = 0;
	virtual Variable* clone() = 0;
};

class Uniform: public Variable{
private:
	double u;
	double x;
	double _step;
	double xmin, xmax;
	std::string inst_name;
	//static std::random_device r;
	static std::default_random_engine e;
	bool initialized;


public:
	double draw();
	double get_value();
	void set_value(double value);
	double get_step();
	void set_step(double new_step);
	std::string get_name();
	Uniform(std::string name, double min, double max, double step);
	Uniform(const Uniform& other);
	Uniform* clone();
};
#endif
