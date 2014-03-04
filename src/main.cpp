// System
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

// Private
#include <Beliefstate.h>

using namespace std;
using namespace beliefstate;


// Global variable for shutdown triggering
Beliefstate* bsBeliefstate;


void printHelp(string strExecutableName) {
  cout << "Usage: " << strExecutableName << " [options]" << endl << endl;
  
  cout << "Available options are:" << endl;
  cout << "  -h, --help\t\tPrint this help" << endl;
  cout << "  -c, --config <file>\tLoad config file <file> instead of the default one" << endl;
  cout << endl;
  
  cout << "Should any questions arise, feel free to send an email to winkler@cs.uni-bremen.de" << endl;
}

void catchSIGTERMandSIGINT(int nSignum) {
  bsBeliefstate->triggerShutdown();
}

int main(int argc, char** argv) {
  // Read command line parameters
  int nC, option_index = 0;
  static struct option long_options[] = {{"config",           required_argument, 0, 'c'},
					 {"help",             no_argument,       0, 'h'},
					 {0,                  0,                 0, 0}};
  
  string strConfigFile = "";
  bool bQuit = false;
  
  while((nC = getopt_long(argc, argv, "c:h", long_options, &option_index)) != -1) {
    switch(nC) {
    case 'c': {
      strConfigFile = string(optarg);
    } break;
      
    case 'h': {
      printHelp(string(argv[0]));
      bQuit = true;
    } break;
      
    default: {
    } break;
    }
  }
  
  if(bQuit == false) {
    bsBeliefstate = new Beliefstate(argc, argv);
    bsBeliefstate->info("Starting beliefstate system.");
    
    Result resInit = bsBeliefstate->init(strConfigFile);
  
    if(resInit.bSuccess) {
      // Catch SIGTERM and SIGINT and bind them to the callback function
      // catchSIGTERMandSIGINT. This will trigger the shutdown mechanism
      // in the Beliefstate instance.
      struct sigaction action;
      memset(&action, 0, sizeof(struct sigaction));
      action.sa_handler = catchSIGTERMandSIGINT;
      sigaction(SIGTERM, &action, NULL);
      sigaction(SIGINT, &action, NULL);
    
      while(bsBeliefstate->cycle()) {
	// Idle here at will.
      }
    } else {
      bsBeliefstate->fail("Initialization of the beliefstate system failed. Being a quitter.");
    }
    
    cout << "\r";
    bsBeliefstate->info("Exiting gracefully.");
    
    bsBeliefstate->deinit();
    
    delete bsBeliefstate;
  }
  
  return EXIT_SUCCESS;
}
