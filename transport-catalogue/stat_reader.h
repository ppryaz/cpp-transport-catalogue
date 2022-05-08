#pragma once
#include "transport_catalogue.h"
#include "geo.h"
#include <string>
#include <vector>
#include <iostream>

namespace transport_catalogue::statistics {

	void StatRequests(std::vector<std::string>& buffer, TransportCatalogue& catalogue);

	void PrintRouteInfo(RouteInfo& route);

	void PrintStopInfo(std::string_view name, TransportCatalogue& catalogue);

	void PrintEmpty(std::string_view type, std::string_view name);
}
