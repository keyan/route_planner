#include <boost/asio.hpp>
#include <string>
#include <utility>

#include "graph_types.h"

using boost::asio::ip::tcp;

// Parse incoming request querystring and return as a string.
std::string parse_querystring(tcp::socket& socket);

// Extract source/target LatLngs from request querystring.
std::pair<LatLng, LatLng> parse_geo_params(std::string& querystring);

// Given routing results construct response JSON and send to client.
void send_response(tcp::socket& socket, Weight& result);

// Run the server on the specified port forever.
void run_server(int port, const char* osm_file);
