#pragma once

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
*/

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>
#include <iostream>

namespace transport_db {

    // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а с другими подсистемами приложения.
    // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)
    class RequestHandler {
    public:
        explicit RequestHandler(const TransportCatalogue& db, map_renderer::MapRenderer& renderer)
            :db_(db), renderer_(renderer) {
        }

        // Возвращает информацию о маршруте (запрос Bus)
        std::optional<const Bus*> GetBusStat(std::string_view bus_name);

        // Возвращает маршруты, проходящие через
        const std::set<std::string_view>* GetBusesByStop(const std::string_view& stop_name) const;

        void RenderMap() const;
        void SetCatalogueDataToRender() const;


    private:
        // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
        const TransportCatalogue& db_;
        map_renderer::MapRenderer& renderer_;

        void SetStopsForRender() const;
        void SetRoutesForRender() const;
    };
} // namespace transport_db