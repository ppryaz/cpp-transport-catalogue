#pragma once
#include "transport_catalogue.h"
#include "geo.h"
#include <string>
#include <vector>
#include <iostream>

namespace transport_catalogue::statistics {

	int ReadLineWithNumber();

	std::string ReadLine();

	std::ostream& StatRequests(std::ostream& os, std::istream& in, TransportCatalogue& catalogue);

	std::vector<std::string> ReadRequest(std::istream& in);

	/*std::ostream& PrintRouteInfo(std::ostream&os, RouteInfo& route);

	std::ostream& PrintStopInfo(std::ostream&os, std::string_view name, TransportCatalogue& catalogue);

	std::ostream& PrintEmpty(std::ostream&os, std::string_view type, std::string_view name);*/
}
