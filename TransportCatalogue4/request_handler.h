#pragma once

/*
 * ����� ����� ���� �� ���������� ��� ����������� �������� � ����, ����������� ������, ������� ��
 * �������� �� �������� �� � transport_catalogue, �� � json reader.
*/

#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>
#include <iostream>

namespace transport_db {

    // ����� RequestHandler ������ ���� ������, ����������� �������������� JSON reader-� � ������� ������������ ����������.
    // ��. ������� �������������� �����: https://ru.wikipedia.org/wiki/�����_(������_��������������)
    class RequestHandler {
    public:
        explicit RequestHandler(const TransportCatalogue& db, map_renderer::MapRenderer& renderer)
            :db_(db), renderer_(renderer) {
        }

        // ���������� ���������� � �������� (������ Bus)
        std::optional<const Bus*> GetBusStat(std::string_view bus_name);

        // ���������� ��������, ���������� �����
        const std::set<std::string_view>* GetBusesByStop(const std::string_view& stop_name) const;

        void RenderMap() const;
        void SetCatalogueDataToRender() const;


    private:
        // RequestHandler ���������� ��������� �������� "������������ ����������" � "������������ �����"
        const TransportCatalogue& db_;
        map_renderer::MapRenderer& renderer_;

        void SetStopsForRender() const;
        void SetRoutesForRender() const;
    };
} // namespace transport_db