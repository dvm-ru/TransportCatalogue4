#include "json.h"

#include <iomanip>

using namespace std;

namespace json {
    /////Node Class Part/////////////////////////////////////////////////////////
    bool operator== (const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }
    bool operator!= (const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }
    /////Type Comprasion Area////////////////////////////////////////////////////
    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }
    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }
    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }
    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);
    }
    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }
    bool Node::IsDouble() const {
        return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this);
    }
    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }
    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }
    /////Get Value Area/////////////////////////////////////////////////////////////////////
    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("Node value is not an Array"s);
        }
        return std::get<Array>(*this);
    }
    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("Node value is not a Map"s);
        }
        return std::get<Dict>(*this);
    }
    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("Node value is not a Bool"s);
        }
        return std::get<bool>(*this);
    }
    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Node value is not an Integer"s);
        }
        return std::get<int>(*this);
    }
    double Node::AsDouble() const {
        if (!std::holds_alternative<int>(*this) && !std::holds_alternative<double>(*this)) {
            throw std::logic_error("Node value is not a Double"s);
        }
        if (std::holds_alternative<int>(*this)) {
            return std::get<int>(*this);
        }
        return std::get<double>(*this);
    }
    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("Node value is not a String"s);
        }
        return std::get<std::string>(*this);
    }

    const NodeValue& Node::GetValue() const {
        return *this;
    }
    /////END of Node Class Part///////////////////////////////////////////////
    /////LoadNode Namespace///////////////////////////////////////////////////
    namespace {
        Node LoadNode(std::istream& input);

        Node LoadString(std::istream& input);

        std::string LoadLiteral(std::istream& input) {
            std::string s;
            while (std::isalpha(input.peek())) {
                s.push_back(static_cast<char>(input.get()));
            }
            return s;
        }

        Node LoadNumber(std::istream& input) {
            std::string parsed_num;

            // Read queue symbol in parsed_num from input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Read one or more number in parsed_num from input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Integer part of number parsing
            if (input.peek() == '0') {
                read_char();
                // After 0 numbers are not allowed in JSON
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Fractional part of a number parsing
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // EXP part of a number parsing
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Trying to convert string to integer
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // In case of fail
                        // code below is trying to convert string into double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadArray(istream& input) {
            Array result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.emplace_back(LoadNode(input));
            }
            if (!input) {
                throw ParsingError("Array parsing error");
            }
            return Node(move(result));
        }

        Node LoadDict(istream& input) {
            Dict dict;
            for (char c; input >> c && c != '}';) {
                if (c == '"') {
                    std::string key = LoadString(input).AsString();
                    if (input >> c && c == ':') {
                        if (dict.find(key) != dict.end()) {
                            throw ParsingError("Duplicate key '"s + key + "' have been found");
                        }
                        dict.emplace(std::move(key), LoadNode(input));
                    }
                    else {
                        throw ParsingError(": is expected but '"s + c + "' has been found"s);
                    }
                }
                else if (c != ',') {
                    throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
                }
            }
            if (!input) {
                throw ParsingError("Dictionary parsing error"s);
            }
            return Node(std::move(dict));
        }

        Node LoadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(std::move(s));
        }

        Node LoadBool(std::istream& input) {
            const auto s = LoadLiteral(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Failed to parse '"s + s + "' as bool"s);
            }
        }

        Node LoadNull(std::istream& input) {
            if (auto literal = LoadLiteral(input); literal == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Failed to parse '"s + literal + "' as null"s);
            }
        }

        Node LoadNode(istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Unexpected EOF"s);
            }

            switch (c) {
            case '[':
                return LoadArray(input);
            case '{':
                return LoadDict(input);
            case '"':
                return LoadString(input);
            case 't':
                // go to next case
                [[fallthrough]];
            case 'f':
                input.putback(c);
                return LoadBool(input);
            case 'n':
                input.putback(c);
                return LoadNull(input);
            default:
                input.putback(c);
                return LoadNumber(input);
            }
        }

    } // namespace
    /////Document Class Part/////////////////////////////////////////////////////////
    bool operator== (const Document& lhs, const Document& rhs) {
        return lhs.root_ == rhs.root_;
    }
    bool operator!= (const Document& lhs, const Document& rhs) {
        return lhs.root_ != rhs.root_;
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }
    const Node& Document::GetRoot() const {
        return root_;
    }
    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }
    /////Std::variant Solution Solver//////////////////////////////////////////
    void NodeValueSolution::operator() (std::nullptr_t) {
        out << "null"s;
    }
    void NodeValueSolution::operator() (const Array& value) {
        out << "["s << std::endl;
        for (auto iter = value.begin(); iter < value.end(); ++iter) {
            std::visit(NodeValueSolution{ out }, iter->GetValue());
            if (value.end() - iter > 1) {
                out << ',' << std::endl;
            }
        }
        out << std::endl << "]"s;
    }//dsf
    void NodeValueSolution::operator() (const Dict& value) {
        out << "{"s << std::endl;
        for (auto iter = value.begin(); iter != value.end(); ++iter) {
            out << '"' << iter->first << '"' << ": "s;
            std::visit(NodeValueSolution{ out }, iter->second.GetValue());
            if (iter != prev(value.end())) {
                out << ","s << std::endl;
            }
        }
        out << std::endl << "}";
    }
    void NodeValueSolution::operator() (bool value) {
        out << std::boolalpha << value;
    }
    void NodeValueSolution::operator() (int value) {
        out << value;
    }
    void NodeValueSolution::operator() (double value) {
        out << value;
    }
    void NodeValueSolution::operator() (const std::string& value) {
        out << "\""s;
        for (const auto& ch : value) { // Escape-symbols converter
            switch (ch) {
            case '"':
                out << "\\\""s;
                break;
            case '\\':
                out << "\\\\"s;
                break;
            case '\n':
                out << "\\n"s;
                break;
            case '\r':
                out << "\\r"s;
                break;
            case '\t':
                out << "\\t"s;
                break;
            default:
                out << ch;
            }
        }
        out << "\""s;
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(NodeValueSolution{ output }, doc.GetRoot().GetValue());
    }

} // namespace js

namespace json {
    /////BUILDER CLASS AREA///////////////////////////////////////////////////////// 
    Node Builder::Build() {
        using namespace std;
        if (node_ended_) { // return Node if it's ended
            return *nodes_stack_.back();
        }
        else {
            throw std::logic_error("Build() is failed. Node is not ended"s);
        }
    }

    Builder::DictValue Builder::Key(std::string key) {
        using namespace std;

        if (nodes_stack_.empty()) { // Key can be added only into initialized Map
            throw std::logic_error("Key() failed. Builder State at EMPTY"s);
        }
        else if (!node_ended_) { // Node is an Array or Map.        
            if (nodes_stack_.back()->IsMap()) {
                nodes_stack_.emplace_back(std::make_unique<Node>(key)); // Add not paired Key in stack. It will wait for Value and after that will be added in Map
            }
            else {
                throw std::logic_error((nodes_stack_.size() > 1) && nodes_stack_.back()->IsString() ? "Key() after Key() (wrong input)"s // stack consist of Map node and Key
                    : "Key() not in a dict node"s); // attempt to add Key into anoter nodevalue (not a Map)
            }
        }
        else { // node_ended = T        
            throw std::logic_error("Key() failed. Node was already ended"s);
        }
        return DictValue(*this); // return value with aux.class type which allows only to add a Value after Key
    }

    Builder& Builder::Value(NodeValue value) {
        using namespace std;

        if (nodes_stack_.empty()) { // Node was not created. Just add value and finish Node creation
            nodes_stack_.emplace_back(std::make_unique<Node>(value));
            node_ended_ = true;
        }
        else if (!node_ended_) { // Node is an Array or Map.
            if (nodes_stack_.back()->IsArray()) { // Array?
                json::Array tmp = nodes_stack_.back()->AsArray();
                tmp.emplace_back(value);
                *nodes_stack_.back() = Node(std::move(tmp));
            }
            else if ((nodes_stack_.size() > 1) && nodes_stack_.back()->IsString()) { // Map? if last element is Key, than create pair Key - Value 
                std::string key = nodes_stack_.back()->AsString(); // Key
                nodes_stack_.pop_back(); // remove Key from storage
                json::Dict dict = nodes_stack_.back()->AsMap(); // tmp Map creation
                dict.insert({ key, value }); // add new Key - Value pair
                *nodes_stack_.back() = Node(std::move(dict)); // overwrite extended Map
            }
            else {
                throw std::logic_error("Value() failed. Dict without a key"s); // Not an Array or Dict without not paired Key 
            }
        }
        else { // node_ended = T              
            throw std::logic_error("Value() failed. Builder State at ENDED value"s);
        }
        return *this; // Method Chaining
    }
    Builder::DictItem Builder::StartDict() {
        using namespace std;
        if (nodes_stack_.empty()) { // Node was not created
            nodes_stack_.emplace_back(std::make_unique<Node>(Dict())); // Add empty map in stack
        }
        else if (!node_ended_) { // Array or Map added in stack
            if (nodes_stack_.back()->IsMap()) { // Start new Map right after Map initialization. (Key is expected)
                throw std::logic_error("StartDict() failed. New Dict inside of another Dict"s);
            }
            else { // Condition is allow to add Map in node
                nodes_stack_.emplace_back(std::make_unique<Node>(Dict()));
            }
        }
        else { // node_ended = T 
            throw std::logic_error("StartDict() failed. Builder State at ENDED value"s);
        }
        return DictItem(*this); // return value with aux.class type which allows only to a Key or end Map
    }
    Builder& Builder::EndDict() {
        using namespace std;
        if (nodes_stack_.empty()) { // Cannot end Map because it was not created before
            throw std::logic_error("EndDict() failed. Dict was not initialized before"s);
        }
        else if (!node_ended_) { // Handle Map
            if (nodes_stack_.back()->IsMap()) {
                if (nodes_stack_.size() == 1) { // Map is ready to be ended. All Key-Value pairs were added in Map before
                    node_ended_ = true;
                }
                else { // stack consist of 2 or more value. Sub Map was created in an Array or main Map  
                    Dict value = nodes_stack_.back()->AsMap();
                    nodes_stack_.pop_back();
                    this->Value(value); // it has to be added as a Value in node
                }
            }
            else {
                throw std::logic_error(nodes_stack_.back()->IsString() ? "EndDict() failed. Dict value expected"s // Map has last Key without a Value
                    : "EndDict() failed. Node is not a Dict"s); // Node is not a Map
            }
        }
        else { // node_ended = T 
            throw std::logic_error("EndDict() failed. Dict has already been ended"s);
        }
        return *this;
    }
    Builder::ArrayItem Builder::StartArray() {
        using namespace std;
        if (nodes_stack_.empty()) { // Node was not created
            nodes_stack_.emplace_back(std::make_unique<Node>(Array()));
        }
        else if (!node_ended_) {
            if (nodes_stack_.back()->IsMap()) { // Start new Array right after Map initialization. (Key is expected)
                throw std::logic_error("StartArray() failed. Builder has started Dict"s);
            }
            nodes_stack_.emplace_back(std::make_unique<Node>(Array())); // Array can be added as an Value of another Array or Map
        }
        else { // node_ended = T 
            throw std::logic_error("StartArray() failed. Builder State at ENDED value"s);
        }
        return ArrayItem(*this);;
    }
    Builder& Builder::EndArray() {
        using namespace std;
        if (nodes_stack_.empty()) { // Cannot end an Array because it was not created before
            throw std::logic_error("EndArray() failded. Array was not initialized before"s);
        }
        else if (!node_ended_) {
            if (nodes_stack_.back()->IsArray()) {
                if (nodes_stack_.size() == 1) { // check if Array is ready to be ended
                    node_ended_ = true;
                }
                else { // End of Sub Array and adding it in main container 
                    json::Array value = nodes_stack_.back()->AsArray();
                    nodes_stack_.pop_back();
                    this->Value(value);
                }
            }
            else {
                throw std::logic_error("EndArray() failded. Builder hasn't started Array before"s);
            }
        }
        else { // node_ended = T
            throw std::logic_error("EndArray() failded. Builder State at ENDED value"s);
        }
        return *this;
    }

    /////AUXILIARY CLASSES AREA/////////////////////////////////////////////////////////  

    Builder::Context::Context(Builder& b)
        :builder_(b)
    { }

    Builder::DictValue Builder::Context::Key(const std::string& key) {
        return builder_.Key(key);
    }
    Builder& Builder::Context::ContValue(const NodeValue& value) {
        return builder_.Value(value);
    }

    Builder::DictItem Builder::Context::StartDict() {
        return builder_.StartDict();
    }
    Builder& Builder::Context::EndDict() {
        return builder_.EndDict();
    }

    Builder::ArrayItem Builder::Context::StartArray() {
        return builder_.StartArray();
    }
    Builder& Builder::Context::EndArray() {
        return builder_.EndArray();
    }

    Builder::ArrayItem::ArrayItem(Builder& b)
        :Context(b)
    { }

    Builder::ArrayItem Builder::ArrayItem::Value(const NodeValue& value) {
        return ArrayItem(ContValue(value)); // Delegate adding value into a node to parent (Context)
    }

    Builder::DictItem::DictItem(Builder& b) // Delegate creation to parent class
        : Context(b) {
    }

    Builder::DictValue::DictValue(Builder& b) // Delegate creation to parent class
        : Context(b) {
    }

    Builder::DictItem Builder::DictValue::Value(const NodeValue& value){
        return Builder::DictItem(ContValue(value)); // Delegate adding value into a node to parent (Context)
    }

} // namespace json
