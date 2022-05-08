#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <tuple>

#include "transport_catalogue.h"
#include "geo.h"

namespace transport_catalogue::input {

	std::string ReadLine();

	int ReadLineWithNumber();

	std::vector<std::string_view> SplitIntoWords(std::string_view text, std::string_view separator);

	std::tuple<std::string_view,
		transport_catalogue::Coordinates,
		std::vector<std::pair<std::string, size_t>>> StopRequest(std::string_view stop);

	std::tuple<std::string_view, std::vector<std::string_view>, bool> RouteRequest(std::string_view route);

	std::pair<std::string, size_t> ParseDistanceBetweenStops(std::string_view);

	std::vector<std::string> FillInputBuffer(std::istream&);

	void Request(std::vector<std::string>& buffer, transport_catalogue::TransportCatalogue& catalogue);
}
