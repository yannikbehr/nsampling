#ifndef NESTEDSAMPLING_H
#define NESTEDSAMPLING_H

#include <vector>
#include "distributions.h"
#include <exception>
#include <memory>

#define PLUS(x,y) (x > y ? x + log(1+std::exp(y-x)) : y + log(1+std::exp(x-y)))


class SamplingException : public std::exception {
};


/*
 * An object holds the information about a sampling point and its
 * log-likelihood and log-weight.
 */
class Object{
public:
	double _logL;
	double _logWt;
	double _logZ;
	double _H;
	std::vector<std::shared_ptr<Variable> > _vars;

	Object(std::vector<std::shared_ptr<Variable> > vars);
	Object(Object& other);
	Object& operator=(const Object& other);
	friend std::ostream& operator<<(std::ostream& os, const Object& o);

	// Draw a new sample from every random variable that is part of the Object
	std::vector<double> draw();

	// Draw a sample around the previous sample from every random variable that
	// is part of the Object using 'step' as a scaling factor
	std::vector<double> trial(double step);

	double get_logL(){return _logL;};
	double get_logWt(){return _logWt;};
	double get_logZ(){return _logZ;};
	double get_H(){return _H;};
	std::vector<double> get_value();
};


/*
 * Hold the results to summarize and return them.
 */
class Result{
public:
	std::vector<std::shared_ptr<Object> > _samples;
	double _logZ, _H;
	int _n, _nvars;
	std::vector<double> _e, _var, _mx;
	std::vector<std::string> _vnames;

	Result(std::vector<std::shared_ptr<Object> > Samples, double LogZ, double H, int n);
	~Result(){};

	// Print a summary of the results to stdout
	void summarize();

	// Return the expectation value for every random variable
	std::vector<double> getexpt(){return _e;};

	// Return the variance for every random variable
	std::vector<double> getvar(){return _var;};

	// Return the maximum for every random variable
	std::vector<double> getmax(){return _mx;};

	// Return random variables' names
	std::vector<std::string> getnames(){return _vnames;};

	// Return the evidence and its standard deviation
	std::vector<double> getZ(){
		std::vector<double> vals;
		vals.push_back(_logZ);
		vals.push_back(std::sqrt(_H/_n));
		return vals;};

	// Return the information gain
	double getH(){return _H;};

	// Return all samples
	std::vector<std::shared_ptr<Object> > get_samples(){return _samples;};

	// Draw a representative set of samples from the posterior
	std::vector<std::shared_ptr<Object> > resample_posterior(int nsamples);
};


/*
 * The main algorithm.
 */
class NestedSampling{
private:
	// The number of MCMC steps to take before accepting
	// a new sample
	int _nsteps;
	// The scale factor for the initial MCMC step
	double _stepscale;
public:
	NestedSampling(int seed=-1);
	~NestedSampling() {};

	// MCMC step to find a new sample 
	void new_sample(Object *Obj, double logLstar,
			const std::function<double (std::vector<double>)> &likelihood);

	// Start the algorithm
	Result* explore(std::vector<std::shared_ptr<Variable> > vars, int initial_samples,
			int maximum_steps,
		       	const std::function<double (std::vector<double>)> &likelihood,
			int mcmc_steps=20,
			double stepscale=0.1,
			double tolZ=1e-3);
};


#endif
