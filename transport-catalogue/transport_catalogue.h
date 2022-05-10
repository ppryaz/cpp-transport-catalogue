#pragma once
#include "geo.h"
#include <algorithm>
#include <deque>
#include <execution>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <string>
#include <string_view>
#include <functional>

namespace transport_catalogue {

	struct Stop
	{
		std::string name;
		Coordinates coordinates;
	};

	struct Bus
	{
		std::string name;
		std::vector<const Stop*> stops;
		bool is_circular;
	};

	struct RouteInfo
	{
		std::string_view name;
		int stops;
		int unique_stops;
		size_t route_length;
		double curvature;
	};

	namespace detail {
	struct DistanceBetweenStopsHasher {
	public:
		size_t operator() (const std::pair<const Stop*, const Stop*> stops_pair) const {
			return hasher(stops_pair.first) + 37 * hasher(stops_pair.second);
		}
	private:
		std::hash<const void*> hasher;
	};
	}

	class TransportCatalogue
	{
	public:
		void AddRoute(std::string_view name, const std::vector<std::string_view>& stops, bool is_circular);
		void AddStop(std::string_view name, 
						Coordinates& coordinates
						);
		RouteInfo GetRouteInfo(std::string_view route);
		const Bus* FindRoute(std::string_view route_name);
		const Stop* FindStop(std::string_view stop_name);
		std::set<std::string> GetStopInfo(std::string_view stop_name);
		void SetDistanceBetweenStops(const Stop* from, const Stop* to, size_t distance);
		size_t GetDistanceBetweenStops(const Stop* from, const Stop* to);

	private:
		std::deque<Bus> buses_;
		std::deque<Stop> stops_;

		std::unordered_map<std::string_view, const Bus*> buses_name_;
		std::unordered_map<std::string_view, const Stop*> stops_name_;
		std::unordered_map<const Stop*, std::set<std::string>> buses_in_stop_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, double, detail::DistanceBetweenStopsHasher> distance_between_stops_;
	};
}
