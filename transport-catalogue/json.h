#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using NodeType = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        Node(Value& value) {
            this->swap(value);
        }
        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsMap() const;

        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const Value& GetValue() const;

        bool operator==(const Node& rhs) const {
            return GetValue() == rhs.GetValue();
        }
        bool operator!=(const Node& rhs) const {
            return !(this->GetValue() == rhs.GetValue());
        }
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& rhs) {
            return (root_ == rhs.GetRoot());
        }
        bool operator!=(const Document& rhs) {
            return !(root_ == rhs.GetRoot());
        }
    private:
        Node root_;

    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);
    void PrintNode(const Node& node, std::ostream& output);
}  // namespace json