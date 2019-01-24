#include <iostream>
#include <limits>
#include <random>
#include <typeinfo>
#include <string>

#include "nested_sampling.h"

Object::Object(std::vector<std::shared_ptr<Variable> > vars){
	std::vector<std::shared_ptr<Variable> >::iterator itv;
	for(itv=vars.begin(); itv !=vars.end(); itv++){
		_vars.push_back(std::shared_ptr<Variable>((*itv)->clone()));
	}
}

Object::Object(Object& other){
	std::vector<std::shared_ptr<Variable> >::iterator itv;
	for(itv=other._vars.begin(); itv !=other._vars.end(); itv++){
		_vars.push_back(std::shared_ptr<Variable>((*itv)->clone()));
	}
	_logL = other._logL;
	_logWt = other._logWt;
	_logZ = other._logZ;
	_H = other._H;
}


Object& Object::operator=(const Object& other){
		if(&other != this) {
			_logL = other._logL;
			_logWt = other._logWt;
			_logZ = other._logZ;
			_H = other._H;
			_vars.clear();
			std::vector<std::shared_ptr<Variable> >::const_iterator itv;
			for(itv=other._vars.begin(); itv !=other._vars.end(); itv++){
				_vars.push_back(std::shared_ptr<Variable>((*itv)->clone()));
			}
		}
		return *this;
}

std::ostream& operator<<(std::ostream& os, const Object& o)
{
  std::vector<std::shared_ptr<Variable> >::const_iterator itv;
  os << "logL: " << o._logL;
  os << "; logWt: " << o._logWt;

  for(itv=o._vars.begin(); itv !=o._vars.end(); itv++){
		os << "; "<<(*itv)->get_name();
		os << ": " <<(*itv)->get_value();
	}
  return os;
}

std::vector<double> Object::draw(){
	std::vector<std::shared_ptr<Variable> >::iterator itv;
	std::vector<double> vals;
	for(itv=_vars.begin(); itv !=_vars.end(); itv++){
		vals.push_back((*itv)->draw());
	}
	return vals;
}

std::vector<double> Object::trial(double step){
	std::vector<std::shared_ptr<Variable> >::iterator itv;
	std::vector<double> vals;
	for(itv=_vars.begin(); itv !=_vars.end(); itv++){
		vals.push_back((*itv)->trial(step));
	}
	return vals;
}

std::vector<double> Object::get_value(){
	std::vector<std::shared_ptr<Variable> >::iterator itv;
	std::vector<double> vals;
	for(itv=_vars.begin(); itv !=_vars.end(); itv++){
		vals.push_back((*itv)->get_value());
	}
	return vals;
}


Result::Result(std::vector<std::shared_ptr<Object> > Samples, double LogZ, double H, int n){
	_samples=Samples;
	_logZ = LogZ;
	_H = H;
	_n = n;
	_nvars = _samples[0]->_vars.size();

	// Compute 1st and 2nd moment
	double w;
	bool lmax = false;

	for(int i=0; i<_nvars; i++){
		_e.push_back(0.0);
		_var.push_back(0.0);
		_mx.push_back(0.0);
		_vnames.push_back(_samples[0]->_vars[i]->get_name());
	}
	_mx.push_back(-std::numeric_limits<double>::max());

	for(uint i=0; i<_samples.size(); i++){
		w = std::exp(_samples[i]->_logWt - _logZ);
		if(_samples[i]->_logL > _mx[_nvars]){
			lmax = true;
			_mx[_nvars] = _samples[i]->_logL;
		}
		for(int j=0; j<_nvars; j++){
			_e[j] += w*_samples[i]->_vars[j]->get_value();
			_var[j] += w*_samples[i]->_vars[j]->get_value()*_samples[i]->_vars[j]->get_value();
			if(lmax)
				_mx[j] = _samples[i]->_vars[j]->get_value();
		}
		lmax = false;
	}
	for(int i=0; i<_nvars; i++){
		_var[i] = _var[i] - _e[i]*_e[i];
	}

}

void Result::summarize(){
	std::cout << "Number of iterates: " << _samples.size();
	std::cout << "; number of initial samples: " << _n << std::endl;
	std::cout << "Evidence: ln(Z) = " << _logZ << "+-" << std::sqrt(_H/_n) << std::endl;
	std::cout << "Information: H = " << _H << " nats = " << _H/log(2.) << std::endl;
	std::cout << "Maximum log-likelihood: "<< _mx[_nvars] << std::endl;
	for(int j=0; j<_nvars; j++){
		std::cout << "E(" << _vnames[j] << ") : " << _e[j];
		std::cout << "; STD(" << _vnames[j] << ") :" << std::sqrt(_var[j]);
		std::cout << "; MAX(" << _vnames[j] << ") :" << _mx[j] << std::endl;
	}
}


std::vector<std::shared_ptr<Object> > Result::resample_posterior(int nsamples){
	double _w_max = -std::numeric_limits<double>::max();
	double u, wt, S=0.;
	int count=0;
	int _nsamples;
	std::random_device _r;
	std::default_random_engine _e = std::default_random_engine(_r());
	std::vector<std::shared_ptr<Object> > new_samples;
	std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);

	for(uint i=0; i<_samples.size(); i++){
		wt = std::exp(_samples[i]->_logWt - _logZ);
		if(wt > _w_max){
			_w_max = wt;
		}
	}
	_nsamples = std::min(nsamples, int(1./_w_max));
	u = uniform_dist(_e);
	for(uint i=0; i<_samples.size(); i++){
		wt = std::exp(_samples[i]->_logWt - _logZ);
		S += _nsamples*wt;
		if(S > u+count && count < _nsamples){
			count++;
			new_samples.push_back(std::make_shared<Object>(*_samples[i]));
		}

	}
	return new_samples;
}


void NestedSampling::new_sample(Object *Obj, double logLstar,
				const std::function<double (std::vector<double>)> &likelihood){
	double step;
	int m;
	int accept = 0;
	int reject = 0;
	Object Try(*Obj);
	std::vector<Variable*> v;
	std::vector<Variable*>::iterator itv;
	m = _nsteps;
	step = _stepscale;
	for(;m>0;m--){
		try{
			Try._logL = likelihood(Try.trial(step));

			if(Try._logL > logLstar){
				*Obj = Try;
				accept++;
			}else{
				// reset to previously accepted sample
				Try = *Obj;
				reject++;
			}
			if(accept > reject)
				step *= exp(1.0/accept);
			if(accept < reject)
				step /=	exp(1.0/reject);
		}catch(SamplingException *e){
		std::cout << "Callback during re-sampling failed" << std::endl;
		m++;
	        }	
	}	
}


Result* NestedSampling::explore(std::vector<std::shared_ptr<Variable> > vars,
		int initial_samples, int maximum_steps,
		const std::function<double (std::vector<double>)> &likelihood,
		int mcmc_steps, double stepscale, double tolZ){
	int i;
	int copy;
	int worst, best;
	int nest;
	double logZnew;
	double logZ = -std::numeric_limits<double>::max();
	double H = 0.0;
	double logLstar;
	double logwidth;
	_nsteps = mcmc_steps;
	_stepscale = stepscale;

	// The following code bit facilitates unit testing
	Variable* pick;
	Variable* tvar = vars[0].get();
	const std::type_info& ti1 = typeid(*tvar);
	const std::type_info& ti2 = typeid(CUniform);
	if(ti1.hash_code() == ti2.hash_code()){
		pick = new CUniform("pick",0,initial_samples);
	} else {
		pick = new Uniform("pick",0,initial_samples);
	}

	std::vector<std::shared_ptr<Object> > Samples;
	Samples.reserve(maximum_steps);
	std::vector<std::shared_ptr<Object> > Obj(initial_samples);

	logwidth = log(1.0 - exp(-1.0/initial_samples));

	for(i=0;i<initial_samples;i++){
		Obj[i] = std::make_shared<Object>(vars);
		try{
			Obj[i]->_logL = likelihood(Obj[i]->draw());
		}catch(SamplingException *e){
			std::cout << "Callback during initialization failed" << std::endl;
			i--;
		}
#ifdef DEBUG
		std::cout <<"Prior: " << i << " " << *Obj[i] <<std::endl;
#endif
	}
	for(nest=0; nest<maximum_steps; nest++){
		// Worst object in collection with Weight = width*Likelihood
		worst = 0;
		best = 0;
		for(i=1; i<initial_samples; i++){
			if(Obj[i]->_logL < Obj[worst]->_logL)
				worst = i;
			if(Obj[i]->_logL > Obj[best]->_logL)
				best = i;
		}

		Obj[worst]->_logWt = logwidth + Obj[worst]->_logL;
		Obj[best]->_logWt = logwidth + Obj[best]->_logL;
		// Update Evidence Z and Information H
		logZnew = PLUS(logZ, Obj[worst]->_logWt);
		H = exp(Obj[worst]->_logWt - logZnew) * Obj[worst]->_logL
				+ exp(logZ - logZnew) * (H + logZ) - logZnew;
		logZ = logZnew;
			
		Obj[worst]->_logZ = logZ;
		Obj[worst]->_H = H;
		// Posterior Samples (optional)
		Samples.push_back(std::make_shared<Object>(*Obj[worst]));
#ifdef DEBUG
		std::cout <<"Samples[nest]: " << *Samples[nest] <<std::endl;
#endif
		if(tolZ*exp(logZ) > exp(Obj[best]->_logWt)){
#ifdef DEBUG
			std::cout << Obj[best]->_logWt << ", " << logZ << std::endl;
#endif
			break;
		}
		// Kill worst object in favour of copy of different survivor
		do copy = (int)(pick->draw()); // force 0 <= copy < n
		while(copy == worst && initial_samples > 1); // don't kill if n is only 1
		logLstar = Obj[worst]->_logL; // new Likelihood constraint
		*Obj[worst] = *Obj[copy]; // overwrite worst object

		// Evolve copied object within constraint
		new_sample(Obj[worst].get(), logLstar, likelihood);
		// Shrink interval
		logwidth -= 1.0/initial_samples;
	}

	Result *rs = new Result(Samples,logZ,H,initial_samples);
	return rs;
}

