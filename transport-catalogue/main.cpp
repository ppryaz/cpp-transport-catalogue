#include <fstream>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "map_renderer.h"

using namespace std::literals;
int main() {
	transport_catalogue::TransportCatalogue db;
    renderer::MapRenderer renderer;
    json_reader::JsonReader input(std::cin, &db, &renderer);
    input.PrintResult(std::cout);
}
