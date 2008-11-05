#include <iostream>
#include <iomanip>
#include "Benchmark.h"
#include "UserAlgorithmTest.h"
#include "MantidAPI/FrameworkManager.h"
//#include "MantidAPI/Workspace.h"
//#include "MantidDataObjects/Workspace1D.h" 
//#include "MantidDataObjects/Workspace2D.h" 

using namespace Mantid::Kernel;
using namespace Mantid::DataObjects;
using namespace Mantid::API;



int main()
{

  FrameworkManagerImpl& fm = FrameworkManager::Instance();

//  UserAlgorithmTest userTest;
 // userTest.RunAllTests();
  
 // Benchmark b;
 // b.RunPlusTest();
    
#if defined _DEBUG
  //NOTE:  Any code in here is temporary for debugging purposes only, nothing is safe!
  //load a raw file
    IAlgorithm* loader = fm.createAlgorithm("LoadRaw");
    loader->setPropertyValue("Filename", "../../../Test/Data/HET15869.RAW");

    std::string outputSpace = "outer";
    loader->setPropertyValue("OutputWorkspace", outputSpace);    

    loader->execute();

    IAlgorithm* focus = fm.createAlgorithm("DiffractionFocussing");
    focus->setPropertyValue("GroupingFileName", "../../../Test/Data/offsets_2006_cycle064.cal");

    std::string resultSpace = "result";
    focus->setPropertyValue("InputWorkspace", outputSpace); 
    focus->setPropertyValue("OutputWorkspace", resultSpace);    

    focus->execute();


#endif


  FrameworkManager::Instance().clear();
  exit(0);
}
