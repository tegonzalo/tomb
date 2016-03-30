/*
 * tomb.cc
 * Created by Tomas Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#include "headers.h"
#include "files.h"
#include "database.h"

using namespace Tomb;

void die()
{
	std::cerr << "TOMB::Wrong sintax or file doesn't exists" << std::endl;
}

bool parsearguments(int argc, char *argv[], std::string &filename, std::string &mode) 
{
	try {
		switch(argc)
		{
			case 2:
				if(!strcmp(argv[1],"-r"))
				{
					mode = "remove";
					return true;
				}
				return false;
			case 3:
				filename = argv[2];
		
				if(!strcmp(argv[1],"-f") and Files::FileExists(filename))
				{
					mode = "file";
					return true;
				}
		
				return false;
			default:
				return false;
		}
		
		return false;
		
	} catch (...) {
		throw;
	}
}

int main(int argc, char *argv[])
{
	try {
		
		std::string filename, mode;
		
		if(parsearguments(argc, argv, filename, mode)) {
			
			if(mode == "remove")
			{
				Files::EmptyDirectory("./models");
			}
			else if(mode == "file")
			{
				JSONNode json = libjson::parse(Files::ReadFileString(filename));
			
				JSONNode::iterator i = json.begin();
				while(i != json.end() and i->name() != "Data") i++;
			
				// Pull all the available info from the database first of all
				Tomb::group_database_fill();
				Tomb::model_database_fill();
				// Empty the temp directory in case there was leftovers from a previous run
				if(Files::IsDirectory("./temp")) Files::EmptyDirectory("./temp");
				std::cout << "DataBases loaded" << std::endl;
			
				Theory theory(*i);
				Model model(theory);
			
				// Get the number of reps
				i = json.begin();
				while(i != json.end() and i->name() != "NReps") i++;
			
				model.generateModelsRec(i->as_int());
			
				//std::cout << RGE::DataBase << std::endl;
				//std::cout << Model::DataBase << std::endl;

				// Flush all the databases to files
				Tomb::model_database_flush();
				Tomb::group_database_flush();
			}
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

