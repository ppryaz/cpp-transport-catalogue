#include <fstream>

#include "transport_catalogue.h"
#include "json_reader.h"
#include "map_renderer.h"

using namespace std::literals;
int main() {
    //test data from file
   // std::ifstream in("D:\\temp\\test4.json"s);
    //std::ofstream out("D:\\temp\\out4.json"s);

    transport_catalogue::TransportCatalogue db;
    renderer::MapRenderer renderer;
    transport_router::TransportRouter router;
    // для ввода данных с консоли   использовать std::cin  
    // ввод  данных из файла - in
    json_reader::JsonReader input(std::cin, &db, &renderer, &router); 
    input.PrintResult(std::cout); // вывод в консоль
    //input.PrintResult(out); //вывод в файл
}
