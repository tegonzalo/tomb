/*
 * tomb.cc
 * Created by Tomas Gonzalo on 12/01/16
 * Last modified on 12/01/16
 */

#include "headers.h"
//#include "files.h"
//#include "group_database.h"

using namespace Tomb;

bool parsearguments(int argc, char *argv[]) 
{
	try {
		if(argc != 3) return false;
		
		//if(!strcmp(argv[1],"-f") and Files::FileExists(std::string(argv[2])))
		//	return true;
		
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

		if(parsearguments(argc, argv)) {
			std::cout << "Good arguments" << std::endl;
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

