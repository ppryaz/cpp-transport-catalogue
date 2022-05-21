#include "json_reader.h"

#include <vector>
#include <string>
#include <sstream>
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
namespace json_reader {
using namespace std::literals;
JsonReader::JsonReader(std::istream &input, DataBasePtr catalogue,
		MapRenderPtr map_renderer) :
		input_(json::Load(input)), catalogue_ptr_(catalogue), map_renderer_ptr_(
				map_renderer) {
	ProcessInput_();
}

void JsonReader::GetOutput(std::ostream &output) {
	Handler::RequestHandler handler(*catalogue_ptr_, *map_renderer_ptr_);
	json::Array result;
	for (const json::Node &request : input_.GetRoot().AsMap().at(
			"stat_requests"s).AsArray()) {
		std::string type = request.AsMap().at("type"s).AsString();
		if (type == "Bus"s) {
			result.push_back(std::move(GetRoutes_(handler, request)));
		}
		if (type == "Stop"s) {
			result.push_back(std::move(GetStops_(handler, request)));
		}
		if (type == "Map"s) {
			result.push_back(std::move(GetMap_(handler, request)));
		}
	}
	json::Document doc(std::move(result));
	json::Print(doc, output);
}

json::Dict JsonReader::GetRoutes_(Handler::RequestHandler &handler,
		const json::Node &request) {
	auto route_info = handler.GetBusStat(
			request.AsMap().at("name"s).AsString());
	int id = request.AsMap().at("id"s).AsInt();
	json::Dict dict;
	dict["request_id"s] = id;
	if (route_info.has_value()) {
		dict["curvature"s] = route_info.value().curvature;
		dict["route_length"s] = route_info.value().route_length;
		dict["stop_count"s] = route_info.value().stops;
		dict["unique_stop_count"s] = route_info.value().unique_stops;
	} else {
		dict["error_message"s] = "not found"s;
	}
	return dict;
}

json::Dict JsonReader::GetStops_(Handler::RequestHandler &handler,
		const json::Node &request) {
	auto stop_info = handler.GetBusesByStop(
			request.AsMap().at("name"s).AsString());
	int id = request.AsMap().at("id"s).AsInt();
	json::Dict dict;
	dict["request_id"] = id;
	if (stop_info) {
		std::set<std::string> buses;
		for (auto *bus : *stop_info) {
			buses.insert(bus->name);
		}
		json::Array buses_arr;
		for (auto &bus : buses) {
			buses_arr.push_back(bus);
		}
		dict["buses"s] = buses_arr;
	} else {
		dict["error_message"s] = "not found"s;
	}
	return dict;
}

json::Dict JsonReader::GetMap_(Handler::RequestHandler &handler,
		const json::Node &request) {
	renderer::RenderSettings render_settings;
	json::Dict readed_settings =
			input_.GetRoot().AsMap().at("render_settings"s).AsMap();
	render_settings.width = readed_settings.at("width"s).AsDouble();
	render_settings.height = readed_settings.at("height"s).AsDouble();
	render_settings.padding = readed_settings.at("padding"s).AsDouble();
	render_settings.line_width = readed_settings.at("line_width"s).AsDouble();
	render_settings.stop_radius = readed_settings.at("stop_radius"s).AsDouble();
	render_settings.bus_label_font_size = readed_settings.at(
			"bus_label_font_size"s).AsInt();
	render_settings.bus_label_offset = svg::Point(
			readed_settings.at("bus_label_offset"s).AsArray()[0].AsDouble(),
			readed_settings.at("bus_label_offset"s).AsArray()[1].AsDouble());
	render_settings.stop_label_font_size = readed_settings.at(
			"stop_label_font_size"s).AsInt();
	render_settings.stop_label_offset = svg::Point(
			readed_settings.at("stop_label_offset"s).AsArray()[0].AsDouble(),
			readed_settings.at("stop_label_offset"s).AsArray()[1].AsDouble());
	if (readed_settings.at("underlayer_color"s).IsString()) {
		render_settings.underlayer_color = readed_settings.at(
				"underlayer_color"s).AsString();
	} else if (readed_settings.at("underlayer_color"s).AsArray().size() == 3) {
		render_settings.underlayer_color = svg::Rgb(
				readed_settings.at("underlayer_color"s).AsArray()[0].AsInt(),
				readed_settings.at("underlayer_color"s).AsArray()[1].AsInt(),
				readed_settings.at("underlayer_color"s).AsArray()[2].AsInt());
	} else {
		render_settings.underlayer_color =
				svg::Rgba(
						readed_settings.at("underlayer_color"s).AsArray()[0].AsInt(),
						readed_settings.at("underlayer_color"s).AsArray()[1].AsInt(),
						readed_settings.at("underlayer_color"s).AsArray()[2].AsInt(),
						readed_settings.at("underlayer_color"s).AsArray()[3].AsDouble());
	}
	render_settings.underlayer_width =
			readed_settings.at("underlayer_width"s).AsDouble();
	for (const auto &color : readed_settings.at("color_palette"s).AsArray()) {
		if (color.IsString()) {
			render_settings.color_palette.push_back(color.AsString());
		} else if (color.AsArray().size() == 3) {
			render_settings.color_palette.push_back(
					svg::Rgb(color.AsArray()[0].AsInt(),
							color.AsArray()[1].AsInt(),
							color.AsArray()[2].AsInt()));
		} else {
			render_settings.color_palette.push_back(
					svg::Rgba(color.AsArray()[0].AsInt(),
							color.AsArray()[1].AsInt(),
							color.AsArray()[2].AsInt(),
							color.AsArray()[3].AsDouble()));
		}
	}
	int id = request.AsMap().at("id"s).AsInt();
	json::Dict dict;
	map_renderer_ptr_->SetRenderSettings(render_settings);
	std::stringstream stream;
	handler.RenderMap().Render(stream);
	dict["request_id"] = id;
	dict["map"s] = json::Node(stream.str());
	return dict;
}

void JsonReader::ProcessInput_() {
	ProcessStops_();
	ProcessRoutes_();
}

void JsonReader::ProcessStops_() {
	//здесь храним все остановки - имена
	std::vector<std::string> stop_names;
	//сюда временно собираем все остановки - и соответствующие им ближайшие остановки с расстоянием
	std::vector<std::pair<std::string, std::vector<std::pair<std::string, int>>>> stops_dist;
	for (const auto &request : input_.GetRoot().AsMap().at("base_requests"s).AsArray()) {
		if (request.AsMap().at("type"s) == "Stop"s) {
			auto request_map = request.AsMap();
			auto name = request_map.at("name"s).AsString();
			auto latitude = request_map.at("latitude"s).AsDouble();
			auto longitude = request_map.at("longitude"s).AsDouble();
			//получаем мапу с ключами - stop  и расстоянием до остановки
			auto road_distance = request_map.at("road_distances"s).AsMap();
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
		auto *from = catalogue_ptr_->FindStop(stop);
		for (auto& [to, distance] : pair_stop_dist) {
			catalogue_ptr_->SetDistanceBetweenStops(from,
					catalogue_ptr_->FindStop(to), distance);
		}
	}
}

void JsonReader::ProcessRoutes_() {
	for (const auto &request : input_.GetRoot().AsMap().at("base_requests"s).AsArray()) {
		if (request.AsMap().at("type"s) == "Bus"s) {
			auto request_map = request.AsMap();
			const auto &name = request_map.at("name"s).AsString();
			std::vector<std::string_view> stops;
			for (const auto &stop : request_map.at("stops"s).AsArray()) {
				stops.push_back(stop.AsString());
			}
			catalogue_ptr_->AddRoute(name, stops,
					request_map.at("is_roundtrip"s).AsBool());
		}
	}
}
}
