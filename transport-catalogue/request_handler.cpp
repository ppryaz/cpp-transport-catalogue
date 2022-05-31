#include "request_handler.h"

namespace Handler {

RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue &db,
		const renderer::MapRenderer &renderer) :
		db_(db), renderer_(renderer) {
}

std::optional<transport_catalogue::RouteInfo> RequestHandler::GetBusStat(
		const std::string_view &bus_name) const {
	return db_.GetRouteInfo(bus_name);
}

const std::unordered_set<domain::BusPtr>* RequestHandler::GetBusesByStop(
		const std::string_view &stop_name) const {
	return db_.GetStopInfo(stop_name);
}

svg::Document RequestHandler::RenderMap() const {
	return renderer_.GetSVG(db_.GetAllRoutes());
}
}
