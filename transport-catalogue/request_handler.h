#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

namespace Handler {

    class RequestHandler {
    public:
        RequestHandler(const transport_catalogue::TransportCatalogue& db,
            const renderer::MapRenderer& renderer, const transport_router::TransportRouter& router);

        std::optional<transport_catalogue::RouteInfo> GetBusStat(const std::string_view& bus_name) const;
        const std::unordered_set<domain::BusPtr>* GetBusesByStop(const std::string_view& stop_name) const;
        std::optional<std::vector<const transport_router::TravelProps*>> GetOptimalRoute(std::string_view from, std::string_view to) const;
        svg::Document RenderMap() const;

    private:
        const transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
        const transport_router::TransportRouter& router_;
    };
} //namespace Handler
