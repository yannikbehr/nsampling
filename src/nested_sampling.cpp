#include <nested_sampling.h>
#include <cmath>
#include <limits>
#include <random>

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

void Object::step_rescale(double factor){
	std::vector<Variable*>::iterator itv;
	for(itv=_vars.begin(); itv !=_vars.end(); itv++){
		(*itv)->set_step((*itv)->get_step()*factor);
	}
}

void Result::summarize(){
	double x = 0.0, xx = 0.0;
	double y = 0.0, yy = 0.0;
	double w;
	int i;
	double xmax, ymax, lmax=-std::numeric_limits<double>::max();
	for(i=0; i<_samples.size(); i++){
		w = std::exp(_samples[i]->logWt - _logZ);
		x += w*_samples[i]->_vars[0]->get_value();
		xx += w*_samples[i]->_vars[0]->get_value()*_samples[i]->_vars[0]->get_value();
		y += w*_samples[i]->_vars[1]->get_value();
		yy += w*_samples[i]->_vars[1]->get_value()*_samples[i]->_vars[1]->get_value();
		if(_samples[i]->logL > lmax){
			lmax = _samples[i]->logL;
			xmax = _samples[i]->_vars[0]->get_value();
			ymax = _samples[i]->_vars[1]->get_value();
		}
	}
	std::cout << "mean(x) = "<< x << ", stddev(x) = "<< std::sqrt(xx-x*x) << std::endl;
	std::cout << "mean(y) = "<< y << ", stddev(y) = "<< std::sqrt(yy-y*y) << std::endl;
	std::cout << "max(x) = "<<xmax<< ", max(y) = "<<ymax<<", max(L) = " << lmax << std::endl;

}

void NestedSampling::new_sample(Object *Obj, double logLstar){
	double step;
	int m = 20;
	int accept = 0;
	int reject = 0;
	Object Try(*Obj);
	std::vector<Variable*> v;
	std::vector<Variable*>::iterator itv;

	for(;m>0;m--){
		Try.logL = _callback->run(Try.draw());
		if(Try.logL > logLstar){
			*Obj = Try;
			accept++;
		}else
			reject++;
		if(accept > reject)
			Try.step_rescale(exp(1.0/accept));
		if(accept < reject)
			Try.step_rescale(1/exp(1.0/reject));
	}
}

Result* NestedSampling::explore(std::vector<Variable*> vars,
		int initial_samples, int maximum_steps){
	int i;
	int copy;
	int worst;
	int nest;
	double logZnew;
	double _logZ = -std::numeric_limits<double>::max();
	double _H = 0.0;
	double logLstar;
	double logwidth;

	std::random_device r;
	//std::default_random_engine e(r());
	std::default_random_engine e;
	std::uniform_int_distribution<int> pick(0,initial_samples-1);
	std::vector<Object*> Samples(maximum_steps);
	std::vector<Object*> Obj(initial_samples);

	logwidth = log(1.0 - exp(-1.0/initial_samples));

	for(i=0;i<initial_samples;i++){
		Obj[i] = new Object(vars);
		Obj[i]->logL = _callback->run(Obj[i]->draw());
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
		logZnew = PLUS(_logZ, Obj[worst]->logWt);
		_H = exp(Obj[worst]->logWt - logZnew) * Obj[worst]->logL
				+ exp(_logZ - logZnew) * (_H + _logZ) - logZnew;
		_logZ = logZnew;
		// Posterior Samples (optional)
		Samples[nest] = new Object(*Obj[worst]);
#ifdef DEBUG
		std::cout <<"++++++++++++++++++++++++++++++++"<<std::endl;
		std::cout <<"Samples[nest]: " << *Samples[nest] <<std::endl;
		std::cout <<"*Obj[worst]: " << *Obj[worst] <<std::endl;
#endif
		// Kill worst object in favour of copy of different survivor
		do copy = pick(e); // force 0 <= copy < n
		while(copy == worst && initial_samples > 1); // don't kill if n is only 1
		logLstar = Obj[worst]->logL; // new Likelihood constraint
		*Obj[worst] = *Obj[copy]; // overwrite worst object
#ifdef DEBUG
		std::cout <<"*Obj[worst]: " << *Obj[worst] <<std::endl;
		std::cout <<"*Obj[copy]: " << *Obj[copy] <<std::endl;
#endif

		// Evolve copied object within constraint
		new_sample(Obj[worst], logLstar);
		// Shrink interval
		logwidth -= 1.0/initial_samples;
	}

	Result *rs = new Result(Samples,_logZ);
	return rs;
}

