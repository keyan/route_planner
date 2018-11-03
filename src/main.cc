#include <iostream>

#include "benchmarks.h"
#include "server.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
  if (argc == 1) {
    cout << "Running benchmarks only..." << endl;
    run_benchmarks();
  } else if (argc == 2) {
    cout << "Running in server mode..." << endl;
    int port = atoi(argv[1]);
    run_server(port);
  } else {
    cout << "Too many parameters provided" << endl;
    exit(1);
  }

  return 0;
}
