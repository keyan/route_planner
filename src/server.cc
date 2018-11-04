#include <algorithm>
#include <boost/asio.hpp>
#include <ctime>
#include <iostream>
#include <unistd.h>

#include "alt.h"
#include "road_network.h"
#include "server.h"
#include "string_util.h"
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

std::pair<LatLng, LatLng> parse_geo_params(std::string& querystring) {
  // Naive request parsing, ignore initial chars until lat/lng.
  size_t pos = querystring.find_first_of("123456789");
  // atof() ignores non-numerical chars so atof("11.11,22.22") -> 11.11
  float source_lat = atof(querystring.substr(pos).c_str());
  pos = querystring.find(',', pos + 1);
  float source_lng = atof(querystring.substr(pos + 1).c_str());
  pos = querystring.find(',', pos + 1);
  float target_lat = atof(querystring.substr(pos + 1).c_str());
  pos = querystring.find(',', pos + 1);
  float target_lng = atof(querystring.substr(pos + 1).c_str());

  return std::make_pair(
      LatLng(source_lat, source_lng), LatLng(target_lat, target_lng));
}

void send_response(tcp::socket& socket, Weight& weight, std::string& polyline) {
  std::stringstream jsonp;
  jsonp << "routingEngineCallback({\n"
        << "  polyline: \"" << escape_json(polyline) << "\",\n"
        << "  travel_time: \"" << ms_to_mins_and_secs(weight) << "\"\n"
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

void run_server(int port, const char* osm_file) {
  // Initialize routing engine
  RoadNetwork road_network = RoadNetwork();
  road_network.load_from_osm_file(osm_file);
  road_network.reduce_to_largest_connected_component();
  Dijkstras dijkstra = Dijkstras(road_network);
  ALT engine = ALT(road_network, dijkstra);

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
      std::pair<LatLng, LatLng> pos = parse_geo_params(querystring);

      NodeID source = geo_position_to_node(road_network, pos.first);
      NodeID target = geo_position_to_node(road_network, pos.second);

      if (source == -1 or target == -1) {
        cout << "Map matching requested markers failed" << endl;
      } else {
        Weight weight = engine.search(source, target);
        if (weight != INF_WEIGHT) {
          std::string polyline = road_network.build_polyline_from_search(
              source, target, dijkstra.shortest_path_tree_);
          send_response(socket, weight, polyline);
        } else {
          cout << "Invalid routing result" << endl;
        }
      }
    }
  } catch (std::exception& e) {
    cerr << e.what() << endl;
  }
}
