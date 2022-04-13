#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {
    using namespace std::literals;
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Number = std::variant<int, double>;
    using NodeValue = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
        friend bool operator== (const Node& lhs, const Node& rhs);
        friend bool operator!= (const Node& lhs, const Node& rhs);
    public:
        // Делаем доступными все конструкторы родительского класса variant
        using variant::variant;
        using Value = variant;

        Node(json::Node::Value& value) {
            this->swap(value);
        }
        /////Type Comprasion Area//////////////////////////////////////////////
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        /////Get Value Area////////////////////////////////////////////////////
        const Array& AsArray() const;
        const Dict& AsMap() const;
        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const NodeValue& GetValue() const;
    };
    /////Document Part/////////////////////////////////////////////////////////
    class Document {
        friend bool operator== (const Document& lhs, const Document& rhs);
        friend bool operator!= (const Document& lhs, const Document& rhs);
    public:
        explicit Document() = default;
        explicit Document(Node root);

        const Node& GetRoot() const;
    private:
        Node root_;
    };

    Document Load(std::istream& input);
    /////Std::variant Solution Solver//////////////////////////////////////////
    struct NodeValueSolution {
        std::ostream& out;
        void operator() (std::nullptr_t);
        void operator() (const Array& value);
        void operator() (const Dict& value);
        void operator() (bool value);
        void operator() (int value);
        void operator() (double value);
        void operator() (const std::string& value);
    };

    void Print(const Document& doc, std::ostream& output);
}  // namespace json

#include <memory>
#include <stack>

namespace json {

	class Builder {
	private:
		/////BUILDER's AUX.CLASSES AREA////////////////////////////////////////////////////
		class DictValue;
		class DictItem;
		class ArrayItem;

		class Context {
		public:
			Context(Builder& b)
				:builder_(b) {
			}

			DictValue Key(const std::string& key); // for DictItem

			DictItem StartDict(); // for ArrayItem and DictValue
			Builder& EndDict(); // for DictItem

			ArrayItem StartArray(); // for Arrayitem and DictValue
			Builder& EndArray(); // for Arrayitem

		protected: // ContValue shouldn't be available to call via Child interface. Only delegation is allowed
			Builder& ContValue(const NodeValue& value); // Problem with 2 different types of Value (for Map and Array). 
														// It's just some king of wrapper to delegate Builder's Value method to inherited classes
		private:
			Builder& builder_;
		};


		class ArrayItem : public Context { // Aux. class for Array. Allows to add Value, start and end Array
		public:
			ArrayItem(Builder& b) // Delegate creation to parent class
				:Context(b) {
			}

			ArrayItem Value(const NodeValue& value) {
				return ArrayItem(ContValue(value)); // Delegate adding value into a node to parent (Context)
			}
			DictValue Key(const std::string& key) = delete; // N/A for an Array
			Builder& EndDict() = delete; // N/A for an Array
		};

		class DictItem : public Context { // Aux. class for Array. Allows to add Value, start and end Array
		public:
			DictItem(Builder& b) // Delegate creation to parent class
				:Context(b) {
			}

			DictItem StartDict() = delete; // Cannot create new Map creation definition instead of Key definition
			ArrayItem StartArray() = delete; // Array can be created only in Map's Value
			Builder& EndArray() = delete; // Only ArrayItem class has right to end an Array
		};

		class DictValue : public Context {
		public:
			DictValue(Builder& b) // Delegate creation to parent class
				:Context(b) {
			}

			DictItem Value(const NodeValue& value) {
				return DictItem(ContValue(value)); // Delegate adding value into a node to parent (Context)
			}

			DictValue Key(const std::string& key) = delete; 
			Builder& EndDict() = delete; 
			Builder& EndArray() = delete; 
		};
		/////BUILDER METHOD AREA////////////////////////////////////////////////////
	public:
		Builder() = default;

		Node Build();

		DictValue Key(std::string key);

		Builder& Value(NodeValue value);

		DictItem StartDict();
		Builder& EndDict();

		ArrayItem StartArray();
		Builder& EndArray();

	private:
		bool node_ended_ = false;
		std::vector<std::unique_ptr<Node>> nodes_stack_;
	};
}
