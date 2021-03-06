#pragma once

#include "domain.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <deque>
#include <cassert>
#include <iostream>
#include <set>

namespace transport_db {
	struct SvSvHasher {
		size_t operator()(const std::pair<std::string_view, std::string_view>& stop_to_stop) const;
		std::hash<std::string_view> sv_hasher;
	};

	using namespace domain;
	class TransportCatalogue {
	public:
		TransportCatalogue() {};
		////////////////////////////////////////////////////////////////////////////////////////////
		void AddRoute(std::string name, const std::vector<std::string_view>& data, bool is_round, std::string_view end_stop);
		const Bus* SearchRoute(std::string_view name) const;

		struct RouteOutput {
			std::string name = {}; // bus name
			size_t real_stops_count = {};
			size_t unique_stops_count = {};
			double route_length = {};
			double curvature = {};
		};
		RouteOutput GetRoute(std::string_view name);
		///////////////////////////////////////////////////////////////////////////////////////////
		void AddStop(std::string name, const geo::Coordinates& coordinates);
		const Stop* SearchStop(std::string_view name) const;

		struct StopOutput {
			std::string name = {}; // stop name
			std::set<std::string_view> buses = {};
		};
		StopOutput GetStop(std::string_view name);
		//////////////////////////////////////////////////////////////////////////////////////////
		void SetDistBtwStops(std::string_view name, std::string_view name_to, const int dist);
		int GetDistBtwStops(std::string_view name, std::string_view name_to);

		//////////////////////////////////////////////////////////////////////////////////////////
		using RoutesMap = std::unordered_map<std::string_view, Bus>;
		const RoutesMap& GetRoutesForRender() const {
			return routes_;
		}

		using StopsMap = std::unordered_map<std::string_view, Stop>;
		const StopsMap& GetStopsForRender() const {
			return stops_;
		}

		using DistMap = std::unordered_map<std::pair<std::string_view, std::string_view>, int, SvSvHasher>;
		const DistMap& GetDistForRouter() const {
			return dist_btw_stops_;
		}
		//////////////////////////////////////////////////////////////////////////////////////////
		~TransportCatalogue() {};
	private:
		std::deque<std::string> stops_names_; // storage for unique STOPS
		std::deque<std::string> buses_names_; // storage for unique BUSES
		
		std::unordered_map<std::pair<std::string_view, std::string_view>, int, SvSvHasher> dist_btw_stops_; // STOP to STOP dist storage

		std::unordered_map<std::string_view, Stop> stops_;
		std::unordered_map<std::string_view, Bus> routes_; // storage for ROUTES with all route STOPS inside
	};
}
