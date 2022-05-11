#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "transport_catalogue.h"
#include "geo.h"

namespace transport_catalogue::statistics {

	int ReadLineWithNumber();

	std::string ReadLine();

	std::ostream& StatRequests(std::ostream& os, std::istream& in, TransportCatalogue& catalogue);

	std::vector<std::string> ReadRequest(std::istream& in);

	} //namespace statistics
