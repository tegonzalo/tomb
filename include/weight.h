/*
 * weight.h
 * Created by T.Gonzalo on 13/08/2013.
 * Last modified on 21/10/2015
 */

#ifndef __WEIGHT_H
#define __WEIGHT_H

#include "headers.h"

namespace Tomb
{
	class Weight : public RVector<double> {
		private:
			//LieGroup *_Group = nullptr;
			std::string _Group = "";
			bool _positive = false;
			int _multiplicity = 0;
			int _level = 0;
		public:
			Weight();
			Weight(const LieGroup &, int);
			Weight(const LieGroup &, const RVector<double> &);
			Weight(const SimpleGroup &, int);
			Weight(const SimpleGroup &, const RVector<double> &);
			Weight(const std::string &, int);
			Weight(const std::string &, const RVector<double> &);
			Weight(const std::string);
			Weight(const JSONNode &);
			Weight(const SimpleGroup &, const double);
			Weight(const Weight &);
			Weight(Weight &&);
			~Weight();
			Weight &operator=(const Weight &);
			Weight &operator=(Weight &&);
			
			std::string id() const;
			//LieGroup &Group() const;
			std::string Group() const;
			bool positive() const;
			int multiplicity() const;
			int level() const;
			void setPositive(bool);
			void setMultiplicity(int);
			void setLevel(int);
			Weight Dual();
			Weight operator+(Weight);
			Weight operator-(Weight);
			Weight operator+(Root);
			Weight operator-(Root);
			Weight operator++(int);
			double operator*(Weight);
			double operator*(Root);
			double operator*(CVector<double>);
			Weight operator*(Matrix<double>);
			bool operator>(const Weight) const;
			bool operator<(const Weight) const;
			Weight Append(Weight);
			JSONNode json(std::string = "") const;
	};

	Weight operator*(Weight, double);
	Weight operator*(double, Weight);
	Weight operator*(Matrix<double>, Weight);
}

#endif /* __WEIGHT_H */