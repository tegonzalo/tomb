/*
 * model.h
 * Created by T. Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#ifndef __MODEL_H
#define __MODEL_H

#include "headers.h"
#include "files.h"

namespace Tomb
{
	class Model : public List<Theory>
	{
		protected:
			
			int _nsteps;
			List<double> _scales;
			
		public:
			
			static List<List<Model> > DataBase;
			
			Model();
			Model(Theory);
			Model(const Model &);
			Model(const List<Theory> &);
			Model(std::string);
			Model(const JSONNode &);
			Model(Model &&);
			~Model();
			Model &operator=(const Model &);
			Model &operator=(const List<Theory> &);
			Model &operator=(Model &&);
			
			int nsteps() const;
			List<double> scales() const;
			
			void DeleteTerm(int);
			void AddTerm(const Theory &);
			
			void generateModels(int = 0, int = 0);
			void generateModelsRec(int = 0, int = 0);
			
			void calculateObservables();
						
			void ParseJSON(const JSONNode &);
	};
	
	std::ostream &operator<<(std::ostream &, const Model &);
}

#endif /* __MODEL_H */