/*
 * model.h
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#ifndef __MODEL_H
#define __MODEL_H

#include "headers.h"

namespace Tomb
{
	class Model : public List<Theory>
	{
		protected:
			
			int _nsteps;
			List<double> _scales;
			
		public:
			
			Model();
			Model(Theory);
			Model(const Model &);
			Model(const List<Theory> &);
			Model(Model &&);
			~Model();
			Model &operator=(const Model &);
			Model &operator=(const List<Theory> &);
			Model &operator=(Model &&);
			
			int nsteps() const;
			List<double> scales() const;
			
			List<Model> generateModels(int = 0, int = 0, bool = false);
	};
	
	std::ostream &operator<<(std::ostream &, const Model &);
}

#endif /* __MODEL_H */