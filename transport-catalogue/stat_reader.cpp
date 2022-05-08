#include "stat_reader.h"

using namespace std::literals;

namespace transport_catalogue::statistics {

	void StatRequests(std::vector<std::string>& buffer, transport_catalogue::TransportCatalogue& catalogue) {
		for (std::string_view str : buffer) {
			if (str.substr(0, "Bus"s.size()) == "Bus"s) {
				str.remove_prefix("Bus "s.size());
				if (catalogue.FindRoute(str) == nullptr) {
					PrintEmpty("Bus"s, str);
				}
				else {
					auto route_info = catalogue.GetRouteInfo(str);
					PrintRouteInfo(route_info);
				}
			}
			else if (str.substr(0, "Stop"s.size()) == "Stop"s) {
				str.remove_prefix("Stop "s.size());
				if (catalogue.FindStop(str) == nullptr) {
					PrintEmpty("Stop"s, str);
				}
				else {
					PrintStopInfo(str, catalogue);
				}
			}
		}
	}

	void PrintRouteInfo(RouteInfo& route) {
		std::cout << "Bus "s << route.name << ": "s
			<< route.stops << " stops on route, "s
			<< route.unique_stops << " unique stops, "s
			<< route.route_length << " route length, "s
			<< route.curvature << " curvature"s << std::endl;
	}

	void PrintStopInfo(std::string_view name, transport_catalogue::TransportCatalogue& catalogue) {
		const std::set<std::string>& stops = catalogue.GetStopInfo(name);
		std::cout << "Stop "s << name << ":"s;
		if (stops.empty()) {
			std::cout << " no buses"s << std::endl;
		}
		else {
			std::cout << " buses"s;
			for (auto& stop : stops) {
				std::cout << ' ' << stop;
			}
			std::cout << std::endl;
		}
	}

	void PrintEmpty(std::string_view type, std::string_view name) {
		std::cout << type << ' ' << name << ": "s << "not found"s << std::endl;
	}
}
