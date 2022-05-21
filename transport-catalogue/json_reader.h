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

		void GetOutput(std::ostream &output);
	private:
		json::Document input_;
		DataBasePtr catalogue_ptr_;
		MapRenderPtr map_renderer_ptr_;

		json::Dict GetRoutes_(Handler::RequestHandler &handler,
				const json::Node &request);
		json::Dict GetStops_(Handler::RequestHandler &handler,
				const json::Node &request);
		json::Dict GetMap_(Handler::RequestHandler &handler,
				const json::Node &request);
		void ProcessInput_();
		void ProcessStops_();
		void ProcessRoutes_();
};
}
