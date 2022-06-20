#include "json_reader.h"
#include "json_builder.h" 

#include <vector>
#include <string>
#include <sstream>

namespace json_reader {
	using namespace std::literals;
	JsonReader::JsonReader(std::istream& input, DataBasePtr catalogue, MapRenderPtr map_renderer,
		RouterPtr transport_router)
		: input_(json::Load(input))
		, catalogue_ptr_(catalogue)
		, map_renderer_ptr_(map_renderer)
		, router_ptr_(transport_router) {
		ProcessInput();
	}

	void JsonReader::PrintResult(std::ostream& output) {
		json::Print(GetOutput(), output);
	}

	json::Document JsonReader::GetOutput() {
		Handler::RequestHandler handler(*catalogue_ptr_, *map_renderer_ptr_, *router_ptr_);
		json::Array result;
		for (const json::Node& request : input_.GetRoot().AsDict().at("stat_requests"s).AsArray()) {
			std::string type = request.AsDict().at("type"s).AsString();
			if (type == "Bus"s) {
				result.push_back(std::move(GetRoutes(handler, request)));
			}
			if (type == "Stop"s) {
				result.push_back(std::move(GetStops(handler, request)));
			}
			if (type == "Map"s) {
				result.push_back(std::move(GetMap(handler, request)));
			}
			if (type == "Route") {
				result.push_back(std::move(GetOptimalRoute(handler, request)));
			}
		}
		return json::Document(std::move(result));
	}

	json::Dict JsonReader::GetRoutes(Handler::RequestHandler& handler, const json::Node& request) {
		auto route_info = handler.GetBusStat(request.AsDict().at("name"s).AsString());
		int id = request.AsDict().at("id"s).AsInt();
		if (route_info.has_value()) {
			return json::Builder{}
				.StartDict()
				.Key("request_id"s)
				.Value(id)
				.Key("curvature"s)
				.Value(route_info.value().curvature)
				.Key("route_length"s)
				.Value(route_info.value().route_length)
				.Key("stop_count"s)
				.Value(route_info.value().stops)
				.Key("unique_stop_count"s)
				.Value(route_info.value().unique_stops)
				.EndDict()
				.Build()
				.AsDict();
		}
		else {
			return json::Builder{}
				.StartDict()
				.Key("request_id"s)
				.Value(id)
				.Key("error_message"s)
				.Value("not found"s)
				.EndDict()
				.Build()
				.AsDict();
		}
	}

	json::Dict JsonReader::GetStops(Handler::RequestHandler& handler, const json::Node& request) {
		auto stop_info = handler.GetBusesByStop(request.AsDict().at("name"s).AsString());
		int id = request.AsDict().at("id"s).AsInt();
		if (stop_info) {
			std::set<std::string> buses;
			for (auto* bus : *stop_info) {
				buses.insert(bus->name);
			}
			json::Array buses_arr;
			for (auto& bus : buses) {
				buses_arr.push_back(bus);
			}
			return json::Builder{}
				.StartDict()
				.Key("request_id"s)
				.Value(id)
				.Key("buses"s)
				.Value(buses_arr)
				.EndDict()
				.Build()
				.AsDict();
		}
		else {
			return json::Builder{}
				.StartDict()
				.Key("request_id"s)
				.Value(id)
				.Key("error_message"s)
				.Value("not found"s)
				.EndDict()
				.Build()
				.AsDict();
		}
	}

	json::Dict JsonReader::GetMap(Handler::RequestHandler& handler, const json::Node& request) {
		renderer::RenderSettings render_settings = ProcessRenderSettings(input_.GetRoot().AsDict().at("render_settings"s).AsDict());
		int id = request.AsDict().at("id"s).AsInt();
		map_renderer_ptr_->SetRenderSettings(render_settings);
		std::stringstream stream;
		handler.RenderMap().Render(stream);
		return json::Builder{}
			.StartDict()
			.Key("request_id"s)
			.Value(id)
			.Key("map"s)
			.Value(stream.str())
			.EndDict()
			.Build()
			.AsDict();
	}

	json::Dict JsonReader::GetOptimalRoute(Handler::RequestHandler& handler, const json::Node& request) {
		int id = request.AsDict().at("id"s).AsInt();
		auto route_details = handler.GetOptimalRoute(request.AsDict().at("from"s).AsString(), request.AsDict().at("to"s).AsString());
		if (!route_details.has_value()) {
			return json::Builder{}
				.StartDict()
				.Key("request_id"s)
				.Value(id)
				.Key("error_message"s)
				.Value("not found"s)
				.EndDict()
				.Build()
				.AsDict();
		}
		json::Array items;
		double total_time = 0.0;
		for (auto* route_item : *route_details) {
			items.push_back(json::Builder{}
				.StartDict()
				.Key("type"s)
				.Value("Wait"s)
				.Key("stop_name"s)
				.Value(route_item->from->name)
				.Key("time"s)
				.Value(route_item->travel_duration.waiting_time / 60)
				.EndDict()
				.Build()
				.AsDict());
			total_time += route_item->travel_duration.waiting_time / 60;
			items.push_back(json::Builder{}
				.StartDict()
				.Key("type"s)
				.Value("Bus"s)
				.Key("bus"s)
				.Value(route_item->route->name)
				.Key("span_count"s)
				.Value(route_item->travel_duration.stops_number)
				.Key("time"s)
				.Value(route_item->travel_duration.travel_time / 60)
				.EndDict()
				.Build()
				.AsDict());
			total_time += route_item->travel_duration.travel_time / 60;
		}
		return json::Builder{}
			.StartDict()
			.Key("request_id"s)
			.Value(id)
			.Key("total_time"s)
			.Value(total_time)
			.Key("items"s)
			.Value(items)
			.EndDict()
			.Build()
			.AsDict();
	}

	renderer::RenderSettings JsonReader::ProcessRenderSettings(const json::Dict& settings) {
		renderer::RenderSettings result;
		result.width = settings.at("width"s).AsDouble();
		result.height = settings.at("height"s).AsDouble();
		result.padding = settings.at("padding"s).AsDouble();
		result.line_width = settings.at("line_width"s).AsDouble();
		result.stop_radius = settings.at("stop_radius"s).AsDouble();
		result.bus_label_font_size = settings.at("bus_label_font_size"s).AsInt();
		result.bus_label_offset = svg::Point(settings.at("bus_label_offset"s).AsArray()[0].AsDouble(),
			settings.at("bus_label_offset"s).AsArray()[1].AsDouble());
		result.stop_label_font_size = settings.at("stop_label_font_size"s).AsInt();
		result.stop_label_offset = svg::Point(settings.at("stop_label_offset"s).AsArray()[0].AsDouble(),
			settings.at("stop_label_offset"s).AsArray()[1].AsDouble());
		result.underlayer_width = settings.at("underlayer_width"s).AsDouble();
		result.underlayer_color = ProcessColor(settings.at("underlayer_color"s));
		for (const auto& color : settings.at("color_palette"s).AsArray()) {
			result.color_palette.push_back(ProcessColor(color));
		}
		return result;
	}

	svg::Color JsonReader::ProcessColor(const json::Node& color) {
		if (color.IsString()) {
			return color.AsString();
		}
		if (color.AsArray().size() == 3) {
			return svg::Rgb(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt());
		}
		return svg::Rgba(color.AsArray()[0].AsInt(), color.AsArray()[1].AsInt(), color.AsArray()[2].AsInt(),
			color.AsArray()[3].AsDouble());
	}

	void JsonReader::ProcessInput() {
		ProcessStops();
		ProcessRoutes();
		ProcessRouterSettings();
	}

	void JsonReader::ProcessStops() {
		std::vector<std::string> stop_names;
		std::vector<std::pair<std::string, std::vector<std::pair<std::string, int>>>> stops_dist;
		for (const auto& request : input_.GetRoot().AsDict().at("base_requests"s).AsArray()) {
			if (request.AsDict().at("type"s) == "Stop"s) {
				auto request_map = request.AsDict();
				auto name = request_map.at("name"s).AsString();
				auto latitude = request_map.at("latitude"s).AsDouble();
				auto longitude = request_map.at("longitude"s).AsDouble();
				auto road_distance = request_map.at("road_distances"s).AsDict();
				stop_names.push_back(name);
				//вектор пар - имя ближайшей останорвки - и расстояние до нее
				std::vector<std::pair<std::string, int>> near_stops;
				for (const auto& [stop_name, distance] : road_distance) {
					near_stops.push_back(
						std::make_pair(stop_name, distance.AsInt()));
				}
				stops_dist.push_back(std::make_pair(name, near_stops));
				catalogue_ptr_->AddStop(name, { latitude, longitude });
			}
		}

		for (auto [stop, pair_stop_dist] : stops_dist) {
			auto* from = catalogue_ptr_->FindStop(stop);
			for (auto& [to, distance] : pair_stop_dist) {
				catalogue_ptr_->SetDistanceBetweenStops(from,
					catalogue_ptr_->FindStop(to), distance);
			}
		}
	}

	void JsonReader::ProcessRoutes() {
		for (const auto& request : input_.GetRoot().AsDict().at("base_requests"s).AsArray()) {
			if (request.AsDict().at("type"s) == "Bus"s) {
				auto request_map = request.AsDict();
				const auto& name = request_map.at("name"s).AsString();
				std::vector<std::string_view> stops;
				for (const auto& stop : request_map.at("stops"s).AsArray()) {
					stops.push_back(stop.AsString());
				}
				catalogue_ptr_->AddRoute(name, stops, request_map.at("is_roundtrip"s).AsBool());
			}
		}
	}

	void JsonReader::ProcessRouterSettings() {
		json::Dict route_settings = input_.GetRoot().AsDict().at("routing_settings"s).AsDict();
		router_ptr_->InitRouter({ route_settings.at("bus_wait_time"s).AsInt(),
			route_settings.at("bus_velocity"s).AsDouble() },
			catalogue_ptr_);
	}
}
