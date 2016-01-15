/*
 * tomb.cc
 * Created by Tomas Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#include "headers.h"
#include "files.h"
#include "group_database.h"

using namespace Tomb;

bool parsearguments(int argc, char *argv[], std::string &filename) 
{
	try {
		if(argc != 3) return false;
		
		filename = argv[2];
		
		if(!strcmp(argv[1],"-f") and Files::FileExists(filename))
			return true;
		
		return false;
		
	} catch (...) {
		throw;
	}
}

void die()
{
	std::cerr << "TOMB::Wrong sintax or file doesn't exists" << std::endl;
}

int main(int argc, char *argv[])
{
	try {
		
		std::string filename;
		
		if(parsearguments(argc, argv, filename)) {
			
			JSONNode json = libjson::parse(Files::ReadFileString(filename));
			
			JSONNode::iterator i = json.begin();
			while(i != json.end() and i->name() != "Data") i++;
			
			// Pull all the available info from the database first of all
			Tomb::database_fill();
			
			std::cout << "DataBases loaded" << std::endl;
			
			Theory theory(*i);
			Model model(theory);
			
			List<Model> models = model.generateModels();
			
			std::cout << models << std::endl;
			
			// Flush all the databases to files
			Tomb::database_flush();
			
		} else {
			die();
		}

	} catch (std::exception &e) {
		std::cerr << "Exception caught: " << e.what() << std::endl;
	} catch (const std::string e) {
		std::cerr << "Exception caught: " << e << std::endl;
	} catch (const char *e) {
		std::cerr << "Exception caught: " << e << std::endl;
	} catch (...) {
		std::cerr << "Exception caught: Unspecified exception" << std::endl;
	}
}

