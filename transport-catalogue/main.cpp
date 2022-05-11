#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <sstream>

using namespace transport_catalogue;

int main() {
	TransportCatalogue transport_catalogue;
	input::Request(std::cin, transport_catalogue);
	statistics::StatRequests(std::cout, std::cin, transport_catalogue);
	return 0;
}
