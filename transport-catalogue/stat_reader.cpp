#include "stat_reader.h"
#include <ostream>

using namespace std::literals;

namespace transport_catalogue::statistics {

	std::ostream& StatRequests(std::ostream&os, std::istream& in, transport_catalogue::TransportCatalogue& catalogue) {

		for (std::string_view str : ReadRequest(in)) {
			if (str.substr(0, "Bus"s.size()) == "Bus"s) {
				str.remove_prefix("Bus "s.size());
				if (catalogue.FindRoute(str) == nullptr) {
					os << "Bus" << ' ' << str << ": "s << "not found"s << '\n';
				}
				else {
					auto route = catalogue.GetRouteInfo(str);
					//os= &PrintRouteInfo(std::cout, route_info);
					os << "Bus "s << route.name << ": "s
								<< route.stops << " stops on route, "s
								<< route.unique_stops << " unique stops, "s
								<< route.route_length << " route length, "s
								<< route.curvature << " curvature"s << std::endl;
				}
			}
			else if (str.substr(0, "Stop"s.size()) == "Stop"s) {
				str.remove_prefix("Stop "s.size());
				if (catalogue.FindStop(str) == nullptr) {
					os << "Stop" << ' ' << str << ": "s << "not found"s << '\n';
				}
				else {
					const std::set<std::string>& stops = catalogue.GetStopInfo(str);
							os << "Stop "s << str << ":"s;
							if (stops.empty()) {
								os << " no buses"s << std::endl;
								 
							}
							else {
								os << " buses"s;
								for (auto& stop : stops) {
									os << ' ' << stop;
								}
								os << std::endl;
								
							}
							
				}
			}
		}
		return os;
	}

	/*std::ostream& PrintRouteInfo(std::ostream&os, RouteInfo& route) {
		os << "Bus "s << route.name << ": "s
			<< route.stops << " stops on route, "s
			<< route.unique_stops << " unique stops, "s
			<< route.route_length << " route length, "s
			<< route.curvature << " curvature"s << std::endl;
		return os;
	}

	std::ostream& PrintStopInfo(std::ostream&os,std::string_view name, transport_catalogue::TransportCatalogue& catalogue) {
		const std::set<std::string>& stops = catalogue.GetStopInfo(name);
		os << "Stop "s << name << ":"s;
		if (stops.empty()) {
			os << " no buses"s << std::endl;
			return os;
		}
		else {
			os << " buses"s;
			for (auto& stop : stops) {
				os << ' ' << stop;
			}
			os << std::endl;
			return os;
		}
		return os;

	}

	std::ostream& PrintEmpty(std::ostream&os,std::string_view type, std::string_view name) {
		os << type << ' ' << name << ": "s << "not found"s << '\n';
		return os;
	}
*/
	std::vector<std::string> ReadRequest(std::istream&) {
	        int request_count = ReadLineWithNumber();
	        std::vector<std::string> requests;
	        while (request_count) {
	            requests.push_back(ReadLine());
	            --request_count;
	        }
	        return requests;
	    }
	std::string ReadLine() {
	        std::string s;
	        std::getline(std::cin, s);
	        return s;
	    }

	int ReadLineWithNumber() {
	        int result;
	        std::cin >> result;
	        ReadLine();
	        return result;
	    }
}
