#include "transport_catalogue.h"

namespace transport_catalogue {

	void TransportCatalogue::AddRoute(std::string_view name, 
										const std::vector<std::string_view>& stops, 
										bool is_circular) {
		std::vector<const Stop*> route_stops;
		route_stops.reserve(stops.size());

		auto& route = buses_.emplace_back(Bus{ std::string(name), route_stops, is_circular });
		for (const auto& stop : stops) {
			const auto* stop_ptr = FindStop(stop);
			route.stops.push_back(stop_ptr);
			buses_in_stop_[stop_ptr].insert(std::string(name));
		}
		buses_name_.insert({ route.name, &route });
	}

	void TransportCatalogue::AddStop(std::string_view name, 
										Coordinates& coordinates
										) 	{
		const auto& stop = stops_.emplace_back(Stop{ std::string(name), coordinates });
		stops_name_.insert({ stop.name, &stop });
		buses_in_stop_[&stop];
	}

	const Bus* TransportCatalogue::FindRoute(std::string_view route_name) {
		const auto it = buses_name_.find(route_name);
		if (it == buses_name_.end()) {
			return nullptr;
		}
		else {
			return it->second;
		}
	}

	const Stop* TransportCatalogue::FindStop(std::string_view stop_name) {
		const auto it = stops_name_.find(stop_name);
		if (it == stops_name_.end()) {
			return nullptr;
		}
		else {
			return it->second;
		}
	}

	RouteInfo TransportCatalogue::GetRouteInfo(std::string_view route) {
		RouteInfo result;
		const auto* route_ptr = FindRoute(route);
		if (route_ptr == nullptr) {
			return {};
		}

		std::vector<const Stop*> stops_local{ route_ptr->stops.begin(), route_ptr->stops.end() };
		std::unordered_set<const Stop*> unique_stops;

		std::for_each(stops_local.begin(), stops_local.end(), [&unique_stops](const auto& stop) {
			if (!unique_stops.count(stop)) {
				unique_stops.insert(stop);
			}
			});

		if (!route_ptr->is_circular) {
			stops_local.insert(stops_local.end(), next(route_ptr->stops.rbegin()), route_ptr->stops.rend());
		}

		double route_length_straight = 0;
		double route_length = 0;
		for (std::vector<const Stop*>::const_iterator it(stops_local.begin()); it != prev(stops_local.end()); ++it) {
			route_length_straight += ComputeDistance((*it)->coordinates, (*(it + 1))->coordinates);
			route_length += GetDistanceBetweenStops(*it, *(it + 1));
		}

		result.name = route_ptr->name;
		result.stops = static_cast<int>(stops_local.size());
		result.unique_stops = static_cast<int>(unique_stops.size());
		result.route_length = route_length;
		result.curvature = route_length / route_length_straight;
		return result;
	}

	std::set<std::string> TransportCatalogue::GetStopInfo(std::string_view stop_name) {
		return buses_in_stop_.at(FindStop(stop_name));
	}

	void TransportCatalogue::SetDistanceBetweenStops(const Stop* from, const Stop* to, size_t distance) {
		auto stops_pair = std::make_pair(from, to);
		distance_between_stops_[stops_pair] = distance;
	}

	size_t TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) {
		auto stops_pair = std::make_pair(from, to);
		if (distance_between_stops_.find(stops_pair) != distance_between_stops_.end()) {
			return distance_between_stops_.at(stops_pair);
		}
		return GetDistanceBetweenStops(to, from);
	}
}
