#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace Handler {

    class RequestHandler {
    public:
        RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer);

        std::optional<transport_catalogue::RouteInfo> GetBusStat(const std::string_view& bus_name) const;
        const std::unordered_set<domain::BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;
        svg::Document RenderMap() const;

    private:
        const transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
    };
} //namespace Handler
