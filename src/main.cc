#include <iostream>

#include "benchmarks.h"
#include "server.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
  if (argc == 1) {
    cout << "Running benchmarks only..." << endl;

    run_benchmarks();
  } else if (argc > 1 and argc < 4) {
    cout << "Running in server mode..." << endl;
    int port = atoi(argv[1]);
    const char* osm_file = "data/seattle.clean.osm";
    if (argc == 3) {
      osm_file = argv[2];
    }

    run_server(port, osm_file);
  } else {
    cout << "Too many parameters provided" << endl;
    exit(1);
  }

  return 0;
}
