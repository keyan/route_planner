#include <algorithm>
#include <boost/asio.hpp>
#include <ctime>
#include <iostream>
#include <unistd.h>

#include "alt.h"
#include "road_network.h"
#include "server.h"
#include "utils.h"

using boost::asio::ip::tcp;
using std::cerr;
using std::cout;
using std::endl;
using std::flush;

std::string parse_querystring(tcp::socket& socket) {
  std::vector<char> request_buffer(1000);
  boost::system::error_code read_error;
  socket.read_some(boost::asio::buffer(request_buffer), read_error);
  std::string request(request_buffer.size(), 0);
  std::copy(request_buffer.begin(), request_buffer.end(), request.begin());
  for (size_t i = 0; i < request.size(); i++) {
    request[i] = isspace(request[i]) ? ' ' : request[i];
  }
  cout << "\"" << request << "\"" << endl;

  return request;
}

void send_response(tcp::socket& socket, Weight& result) {
  std::stringstream jsonp;
  jsonp << "routingEngineCallback({\n"
        << "  polyline: \"" << "_cnnGpww}Luy@rcC" << "\",\n"
        << "  travel_time: \"" << ms_to_mins_and_secs(result) << "\"\n"
        // << [" << sourceLat << "," << sourceLng << "," << targetLat
        // << "," << targetLng << "]\n"
        << "})\n";
  std::ostringstream answer;
  answer << "HTTP/1.1 200 OK\r\n"
         << "Content-Length: " << jsonp.str().size() << "\r\n"
         << "Content-Type: application/javascript"
         << "\r\n"
         << "Connection: close\r\n"
         << "\r\n"
         << jsonp.str();
  boost::system::error_code write_error;
  boost::asio::write(
      socket, boost::asio::buffer(answer.str()), boost::asio::transfer_all(),
      write_error);
}

void run_server(int port) {
  // Initialize routing engine
  // RoadNetwork road_network = RoadNetwork();
  // road_network.load_from_osm_file("data/burlington.osm");
  // road_network.reduce_to_largest_connected_component();
  // Dijkstras dijkstra = Dijkstras(road_network);
  // ALT engine = ALT(road_network, dijkstra);

  try {
    // Create socket and begin listening on port for requests.
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

    int i = 0;
    while (true) {
      // Wait for request.
      cout << "\x1b[1m\x1b[34m[" << (++i) << "] Waiting for query on port "
           << port << " ... \x1b[0m" << flush;
      tcp::socket socket(io_service);
      acceptor.accept(socket);
      std::time_t now = std::time(0);
      std::string daytime = std::ctime(&now);
      cout << "received request -- " << daytime << flush;

      std::string querystring = parse_querystring(socket);

      // Naive request parsing, ignore initial 5 chars "GET /".
      size_t pos = 5;
      float source_lat = atof(querystring.substr(pos + 1).c_str());
      pos = querystring.find(',', pos + 1);
      float source_lng = atof(querystring.substr(pos + 1).c_str());
      pos = querystring.find(',', pos + 1);
      float target_lat = atof(querystring.substr(pos + 1).c_str());
      pos = querystring.find(',', pos + 1);
      float target_lng = atof(querystring.substr(pos + 1).c_str());

      // Weight result = engine.search(origin.id_, target.id_);
      Weight result = 1012340;

      send_response(socket, result);
    }
  } catch (std::exception& e) {
    cerr << e.what() << endl;
  }
}
