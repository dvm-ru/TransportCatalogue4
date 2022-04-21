#pragma once

#include <istream>
#include <iostream>
#include <stdexcept>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace json {
    // ��� ������ ������ ������������� ��� ������� �������� JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;
    public:
        //������� �������� �������� variant (std::get<...>)
        int AsInt() const;
        double AsDouble() const;
        bool AsBool() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsDict() const;
        const Dict& AsMap() const;
        //���������, ��� �� ��� � �������� variant? (std::holds_alternative<...>)
        bool IsInt() const;
        bool IsPureDouble() const;
        bool IsDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsDict() const;
        bool IsMap() const;
        bool IsNull() const;
        Value& GetValue();
        const Value& GetValue() const;
    };
    bool operator==(const Node& lhs, const Node& rhs);
    bool operator!=(const Node& lhs, const Node& rhs);

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);
        const Node& GetRoot() const;
    private:
        Node root_;
    };
    bool operator== (const Document& lhs, const Document& rhs);
    bool operator!= (const Document& lhs, const Document& rhs);
    
    // ----------------------------------------------------------------------------
    Document Load(std::istream& input);
    void Print(const Document& doc, std::ostream& output);
}  // namespace json