#ifndef NESTEDSAMPLING_H
#define NESTEDSAMPLING_H

#include <vector>
#include <distributions.h>
#include <exception>

#define PLUS(x,y) (x > y ? x + log(1+std::exp(y-x)) : y + log(1+std::exp(x-y)))


class SamplingException : public std::exception {
};


/*
 *  Callback function that does the likelihood computation.
 */
class Callback {
public:
        virtual ~Callback() {}

        // Evaluate the likelihood function
        virtual double run(std::vector<double> vals) { return 0.0; }
};


/*
 * An object holds the information about a sampling point and its
 * log-likelihood and log-weight.
 */
class Object{
public:
	double _logL;
	double _logWt;
	std::vector<Variable*> _vars;

	Object(std::vector<Variable*> vars);
	Object(Object& other);
	Object& operator=(const Object& other);
	friend std::ostream& operator<<(std::ostream& os, const Object& o);

	// Draw a new sample from every random variable that is part of the Object
	std::vector<double> draw();

	// Draw a sample around the previous sample from every random variable that
	// is part of the Object using 'step' is a scaling factor
	std::vector<double> draw(double step);
};


/*
 * Hold the results to summarize and return them.
 */
class Result{
public:
	std::vector<Object*> _samples;
	double _logZ, _H;
	int _n, _nvars;
	std::vector<double> _e, _var, _mx;
	std::vector<std::string> _vnames;

	Result(std::vector<Object*> Samples, double LogZ, double H, int n);
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
	std::vector<Object*> get_samples(){return _samples;};

	// Draw a representative set of samples from the posterior
	std::vector<Object*> resample_posterior(int nsamples);
};


/*
 * The main algorithm.
 */
class NestedSampling{
private:
        Callback *_callback;
	// The number of MCMC steps to take before accepting
	// a new sample
	int _nsteps;
	// The scale factor for the initial MCMC step
	double _stepscale;
public:
	NestedSampling(): _callback(0){};
	~NestedSampling() { delCallback(); };
	void delCallback() { delete _callback; _callback = 0; };

	// Setup the callback function which computes the log-likelihood value
	void setCallback(Callback *cb) { delCallback(); _callback = cb; };
	void new_sample(Object *Obj, double logLstar);

	// Start the algorithm after the setup is done.
	Result* explore(std::vector<Variable*> vars, int initial_samples,
			int maximum_steps, int mcmc_steps=20,
			double stepscale=0.1);
};


#endif
