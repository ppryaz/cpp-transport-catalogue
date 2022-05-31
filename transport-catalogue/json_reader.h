#pragma once

#include "transport_catalogue.h"
#include "request_handler.h"
#include "json.h"
#include "domain.h"
#include "map_renderer.h"

namespace json_reader {

using DataBasePtr = transport_catalogue::TransportCatalogue*;
using MapRenderPtr = renderer::MapRenderer*;
class JsonReader {
	public:
		explicit JsonReader(std::istream &input, DataBasePtr catalogue,
				MapRenderPtr map_renderer);

		json::Document GetOutput();
		void PrintResult(std::ostream& output);
	private:
		json::Document input_;
		DataBasePtr catalogue_ptr_;
		MapRenderPtr map_renderer_ptr_;

		json::Dict GetRoutes(Handler::RequestHandler &handler,
				const json::Node &request);
		json::Dict GetStops(Handler::RequestHandler &handler,
				const json::Node &request);
		json::Dict GetMap(Handler::RequestHandler &handler,
				const json::Node &request);
		renderer::RenderSettings ProcessRenderSettings(const json::Dict& settings);
		svg::Color ProcessColor(const json::Node& color);
		void ProcessInput();
		void ProcessStops();
		void ProcessRoutes();
};
}
