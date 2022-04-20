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
    // задаём русский текст
    std::setlocale(LC_ALL, "Russian");
    std::system("chcp 1251");
#endif

    //json::Print(
    //    json::Document{
    //        json::Builder{}
    //        .StartDict()
    //            .Key("key1"s).Value(123)
    //            .Key("key2"s).Value("value2"s)
    //            .Key("key3"s).StartArray()
    //                .Value(456)
    //                .StartDict().EndDict()
    //                .StartDict()
    //                    .Key(""s)
    //                    .Value(nullptr)
    //                .EndDict()
    //                .Value(""s)
    //            .EndArray()
    //        .EndDict()
    //        .Build()
    //    },
    //    cout
    //);
    //cout << endl;
    // 
    //json::Print(
    //    json::Document{
    //        json::Builder{}
    //        .Value("just a string"s)
    //        .Build()
    //    },
    //    cout
    //);
    //cout << endl;

    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив с ответами.
     * Вывести в stdout ответы в виде JSON
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
