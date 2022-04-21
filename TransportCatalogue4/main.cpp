#include "json_reader.h"
#include "request_handler.h"

#include <iostream>
#include <random>
#include <fstream>
#include <string_view>

using namespace std;
using namespace std::literals::string_literals;

int main() {
#ifdef AVRGR
    // ����� ������� �����
    std::setlocale(LC_ALL, "Russian");
    std::system("chcp 1251");
#endif
    /*
     * ��������� ��������� ���������:
     *
     * ������� JSON �� stdin
     * ��������� �� ��� ������ JSON ���� ������ ������������� �����������
     * ��������� ������� � �����������, ����������� � ������� "stat_requests", �������� JSON-������ � ��������.
     * ������� � stdout ������ � ���� JSON
     */

    transport_db::TransportCatalogue catalog;
    map_renderer::MapRenderer renderer;
    json_reader::JsonReader json(catalog, renderer);
    json.ReadInput(std::cin);
    json.FillCatalogue();

    //json.ReadInput(std::cin);
    //map_renderer::MapRenderer renderer;
    transport_db::RequestHandler request_handler(catalog, renderer);
    json.PrintRequests(std::cout, request_handler);
}
