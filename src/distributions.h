#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <string>
#include <random>

class Variable{
public:
	virtual double draw() = 0;
	virtual double draw(double step) = 0;
	virtual double get_value() = 0;
	virtual void set_value(double value) = 0;
	virtual std::string get_name() = 0;
	virtual Variable* clone() = 0;
};

class Uniform: public Variable{
private:
	double u;
	double x;
	double xmin, xmax;
	std::string inst_name;
	static std::default_random_engine e;


public:
	double draw();
	double draw(double step);
	double get_value();
	void set_value(double value);
	std::string get_name();
	Uniform(std::string name, double min, double max);
	Uniform(const Uniform& other);
	Uniform* clone();
};

class CUniform: public Variable{
private:
	double u;
	double x;
	double xmin, xmax;
	std::string inst_name;


public:
	double draw();
	double draw(double step);
	double get_value();
	void set_value(double value);
	std::string get_name();
	CUniform(std::string name, double min, double max);
	CUniform(const CUniform& other);
	CUniform* clone();
};
#endif
