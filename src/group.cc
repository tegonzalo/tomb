/*
* group.cc
* Created by T.Gonzalo on 15/02/2012.
* Last modified on 12/07/2012..
*/

#include "headers.h"
#include "files.h"
#include "database.h"

using namespace Tomb;


int main(int argc, char *argv[]) {

  try {
        
    int rank, maxdim = 50;
    char type;
    std::string id;
    
    switch(argc) {
      case 2:
        if(!strcmp(argv[1],"clean")) {
          Files::EmptyDirectory("./out");
          return 0;
        } else {
          std::string s(argv[1]);
          if(s.find('x') != std::string::npos) {
            id = s;
          } else {
            type = argv[1][0];
            char *auxstring = strndup(argv[1]+1, strlen(argv[1])-1);
            rank = atoi(auxstring);
            free(auxstring);
          }
        }
        break;
        
      case 3:
        if(!strcmp(argv[1],"clean")) {
          std::string dir = "./out/";
          dir.append(argv[2]);
          if(Files::IsDirectory(dir)) {
            Files::DeleteDirectory(dir);
            return 0;
          } else {
            throw "Directory does not exits";
          }
        } else {
          std::string s(argv[1]);
          if(s.find('x') != std::string::npos) {
            id = s;
          } else {
            type = argv[1][0];
            char *auxstring = strndup(argv[1]+1, strlen(argv[1])-1);
            rank = atoi(auxstring);
            free(auxstring);
          }
          maxdim = atoi(argv[2]);
        }
        break;
        
      case 4:
        if(!strcmp(argv[1],"clean")) {
          std::string dir = "./out/";
          dir.append(argv[2]);
          dir.append("/Reps/");
          dir.append(argv[3]);
          dir.append(".out");
          if(Files::FileExists(dir)) {
            Files::DeleteFile(dir);
            return 0;
          } else {
            throw "File does not exits";
          }
        } else {
          throw "Wrong syntax, see manual for further information";
        }
        break;
        
      default:
        throw "Wrong syntax, see manual for further information";

      }

    if(id.empty()) {
      if(type != 'U' and type != 'A' and type != 'B' and type != 'C' and type != 'D' and type != 'E' and type != 'F' and type != 'G') {
        throw "main::Group not valid";
      }

        if(rank <= 0) {
        throw "main::Rank must be a positive number";
        }
      
        if(rank == 2 and type == 'D') {
        throw "main::D2 is not a simple group";
        return 1;
        }

      if(rank >= 10) {
        std::cout << "Rank is too high, are you sure you want to continue?(Y/N)" << std::endl;
        char yesno;
        std::cin >> yesno;
        if(toupper(yesno) == 'N') {
          return 1;
        }
      }
    }
    
    if(maxdim <=0) {
      throw "main::Dim must be a positive number";
    }
    
    // Pull all the available info from the database first of all
    Tomb::group_database_fill();
    
    if(id.empty()) {
      SimpleGroup G1(rank, type);
      
      id = G1.id();
      
    }
    
    std::cout << "Group : " << id << std::endl;
    LieGroup G(id);

    std::cout << "Calculating group info..." << std::endl;
      
    std::cout << "Casimir = " << G.Casimir() << std::endl;

    // Reps
    List<Rrep> Reps = G.Reps(maxdim);
    std::cout << "Reps = " << Reps << std::endl;

    // Subgroups
    List<SubGroup> MaximalSubgroups = G.MaximalSubgroups();
    std::cout << "Maximal subgroups = " << MaximalSubgroups << std::endl;

    List<SubGroup> Subgroups = G.Subgroups();
    std::cout << "Subgroups = " << Subgroups << std::endl;
    //std::cout << Subgroups.json().write_formatted() << std::endl;
    
    // Decomposition of reps
    for(int j=0; j<Reps.nterms(); j++) {
      for(int i=0; i<MaximalSubgroups.nterms(); i++) {
        Sum<Rrep> reps = Reps.GetObject(Reps.nterms()-1-j).Decompose(MaximalSubgroups.GetObject(i));
        std::cout << Reps.GetObject(Reps.nterms()-1-j) << "(" << G << ") -> " << reps << "(" << MaximalSubgroups.GetObject(i) << ")" << std::endl;
      }
    }
    
    // Breaking Chains
/*		LieGroup SMGroup("A2xA1xU1");

    std::cout << "Breaking chains of " << G << " to " << SMGroup << ":" << std::endl;
    std::cout << G.BreakingChains(SMGroup) << std::endl;


    // Product of reps
    for(int i=0; i<Reps.nterms(); i++) {
      for(int j=i; j<Reps.nterms(); j++) {
        std::cout << Reps.GetObject(i) << " * " << Reps.GetObject(j) << " = " << Reps.GetObject(i)*Reps.GetObject(j) << std::endl;
      }
    }
/*
    
/*		
    // Invariants
    List<Rrep> ListofReps;
    if(Reps.nterms() > 1) ListofReps.AddTerm(Reps.GetObject(1));
    if(Reps.nterms() > 2) ListofReps.AddTerm(Reps.GetObject(2));
    if(Reps.nterms() > 3) ListofReps.AddTerm(Reps.GetObject(3));
    if(Reps.nterms() > 4) ListofReps.AddTerm(Reps.GetObject(4));
    
    
    std::cout << "Calculating Invariants ... " << std::endl;
    List<Product<Rrep> > Invariants = G.Invariants(ListofReps,4);

    std::cout << "Invariants = " << Invariants << std::endl;
*/
    // Flush all the databases to files
    Tomb::group_database_flush();

    return 1;
    
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



//#endif
