/*
 * database.h
 * Created by T.Gonzalo on 10/11/2015.
 * Last modified on 03/12/2015
 */

#ifndef __DATABASE_H
#define __DATABASE_H

#include <map>
#include "headers.h"
#include "files.h"
#include "libjson.h"

namespace Tomb
{
	
	// Fills the group database with info from the files
	void group_database_fill();
	
	// Flush the group database into json files
	void group_database_flush();

	// Fills the model database with info from the files
	void model_database_fill();
	
	// Flush the model database into json files
	void model_database_flush();

	// Filter the database with the observables
 	void model_database_filter(List<std::string> &);
}

#endif /* __DATABASE_H */