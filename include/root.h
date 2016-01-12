/*
 * root.h
 * Created by T.Gonzalo on 13/08/2013.
 * Last modified on 21/10/2015
 */

#ifndef __ROOT_H
#define __ROOT_H

#include "headers.h"

namespace Tomb 
{
	class Root : public RVector<double> {
		
		private:
			SimpleGroup *_Group = NULL;
			double _length = 0;
		public:
			Root(const SimpleGroup &, int);
			Root(const SimpleGroup &, const RVector<double> &);
			Root(const std::string);
			Root(const Root &);
			Root(Root &&);
			~Root();
			Root operator=(const Root &);
			Root operator=(Root &&);
			
			std::string id() const;
			SimpleGroup &Group() const;
			double calculateLength();
			double length() const;
			Root Dual();
			Root operator+(Root);
			Root operator-(Root);
			double operator*(Root);
			double operator*(CVector<double>);
			Root operator*(Matrix<double>);
			
	};

	Root operator*(Root, double);
	Root operator*(double, Root);
}

#endif /* __ROOT_H */