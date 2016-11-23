#include <nested_sampling.h>
#include <cmath>
#include <limits>
#include <random>
#include <cfloat>
#include <typeinfo>
#include <string>


Object::Object(std::vector<Variable*> vars){
	std::vector<Variable*>::iterator itv;
	for(itv=vars.begin(); itv !=vars.end(); itv++){
		_vars.push_back((*itv)->clone());
	}
}

Object::Object(Object& other){
	std::vector<Variable*>::iterator itv;
	for(itv=other._vars.begin(); itv !=other._vars.end(); itv++){
		_vars.push_back((*itv)->clone());
	}
	logL = other.logL;
	logWt = other.logWt;
}


Object& Object::operator=(const Object& other){
		if(&other != this) {
			logL = other.logL;
			logWt = other.logWt;
			_vars.clear();
			std::vector<Variable*>::const_iterator itv;
			for(itv=other._vars.begin(); itv !=other._vars.end(); itv++){
				_vars.push_back((*itv)->clone());
			}
		}
		return *this;
}

std::ostream& operator<<(std::ostream& os, const Object& o)
{
  std::vector<Variable*>::const_iterator itv;
  os << "logL: " << o.logL;
  os << "; logWt: " << o.logWt;

  for(itv=o._vars.begin(); itv !=o._vars.end(); itv++){
		os << "; "<<(*itv)->get_name();
		os << ": " <<(*itv)->get_value();
	}
  return os;
}

std::vector<double> Object::draw(){
	std::vector<Variable*>::iterator itv;
	std::vector<double> vals;
	for(itv=_vars.begin(); itv !=_vars.end(); itv++){
		vals.push_back((*itv)->draw());
	}
	return vals;
}

std::vector<double> Object::draw(double step){
	std::vector<Variable*>::iterator itv;
	std::vector<double> vals;
	for(itv=_vars.begin(); itv !=_vars.end(); itv++){
		vals.push_back((*itv)->draw(step));
	}
	return vals;
}


Result::Result(std::vector<Object*> Samples, double LogZ, double H, int n){
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

	for(int i=0; i<_samples.size(); i++){
		w = std::exp(_samples[i]->logWt - _logZ);
		if(_samples[i]->logL > _mx[_nvars]){
			lmax = true;
			_mx[_nvars] = _samples[i]->logL;
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


void NestedSampling::new_sample(Object *Obj, double logLstar){
	double step=0.1;
	int m = 20;
	int accept = 0;
	int reject = 0;
	Object Try(*Obj);
	std::vector<Variable*> v;
	std::vector<Variable*>::iterator itv;

	for(;m>0;m--){
		Try.logL = _callback->run(Try.draw(step));
		if(Try.logL > logLstar){
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
	}
}

Result* NestedSampling::explore(std::vector<Variable*> vars,
		int initial_samples, int maximum_steps){
	int i;
	int copy;
	int worst;
	int nest;
	double logZnew;
	double logZ = -std::numeric_limits<double>::max();
	double H = 0.0;
	double logLstar;
	double logwidth;

	// The following code bit facilitates unit testing
	Variable* pick;
	Variable* tvar = vars[0];
	const std::type_info& ti1 = typeid(*tvar);
	const std::type_info& ti2 = typeid(CUniform);
	if(ti1.hash_code() == ti2.hash_code()){
		pick = new CUniform("pick",0,initial_samples);
	} else {
		pick = new Uniform("pick",0,initial_samples);
	}

	std::vector<Object*> Samples(maximum_steps);
	std::vector<Object*> Obj(initial_samples);

	logwidth = log(1.0 - exp(-1.0/initial_samples));

	for(i=0;i<initial_samples;i++){
		Obj[i] = new Object(vars);
		Obj[i]->logL = _callback->run(Obj[i]->draw());
#ifdef DEBUG
		std::cout <<"Prior: " << i << " " << *Obj[i] <<std::endl;
#endif
	}
	for(nest=0; nest<maximum_steps; nest++){
		// Worst object in collection with Weight = width*Likelihood
		worst = 0;
		for(i=1; i<initial_samples; i++){
			if(Obj[i]->logL < Obj[worst]->logL)
				worst = i;
		}

		Obj[worst]->logWt = logwidth + Obj[worst]->logL;
		// Update Evidence Z and Information H
		logZnew = PLUS(logZ, Obj[worst]->logWt);
		H = exp(Obj[worst]->logWt - logZnew) * Obj[worst]->logL
				+ exp(logZ - logZnew) * (H + logZ) - logZnew;
		logZ = logZnew;
		// Posterior Samples (optional)
		Samples[nest] = new Object(*Obj[worst]);
#ifdef DEBUG
		std::cout <<"Samples[nest]: " << *Samples[nest] <<std::endl;
#endif
		// Kill worst object in favour of copy of different survivor
		do copy = (int)(pick->draw()); // force 0 <= copy < n
		while(copy == worst && initial_samples > 1); // don't kill if n is only 1
		logLstar = Obj[worst]->logL; // new Likelihood constraint
		*Obj[worst] = *Obj[copy]; // overwrite worst object

		// Evolve copied object within constraint
		new_sample(Obj[worst], logLstar);
		// Shrink interval
		logwidth -= 1.0/initial_samples;
	}

	Result *rs = new Result(Samples,logZ,H,initial_samples);
	return rs;
}

