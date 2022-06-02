// Copyright 2022, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <curl/curl.h>

#include <iostream>

#include "qlever-petrimaps/server/Server.h"
#include "util/Misc.h"
#include "util/http/Server.h"
#include "util/log/Log.h"

using petrimaps::Server;

// _____________________________________________________________________________
void printHelp(int argc, char** argv) {
  UNUSED(argc);
  std::cout << "Usage: " << argv[0] << " [-p <port>] [--help] [-h]"
            << "\n";
  std::cout
      << "\nAllowed arguments:\n    -p <port>  Port for server to listen to\n"
      << "\n    -m <memory>  Max memory in GB\n";
}

// _____________________________________________________________________________
int main(int argc, char** argv) {
  // disable output buffering for standard output
  setbuf(stdout, NULL);

  // initialize randomness
  srand(time(NULL) + rand());  // NOLINT

  // init CURL
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // default port
  int port = 9090;
  double maxMemoryGB = (sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) * 0.9) / 1000000000 ;

  for (int i = 1; i < argc; i++) {
    std::string cur = argv[i];
    if (cur == "-h" || cur == "--help") {
      printHelp(argc, argv);
      exit(0);
    } else if (cur == "-p") {
      if (++i >= argc) {
        LOG(ERROR) << "Missing argument for port (-p).";
        exit(1);
      }
      port = atoi(argv[i]);
    } else if (cur == "-m") {
      if (++i >= argc) {
        LOG(ERROR) << "Missing argument for max memory (-m).";
        exit(1);
      }
      maxMemoryGB = atof(argv[i]);
    }
  }

  LOG(INFO) << "Starting server...";
  LOG(INFO) << "Max memory is " << maxMemoryGB << " GB...";
  Server serv(maxMemoryGB * 1000000000);

  LOG(INFO) << "Listening on port " << port;
  util::http::HttpServer(port, &serv, std::thread::hardware_concurrency())
      .run();
}
