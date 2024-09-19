#include <charconv>
#include <optional>
#include <regex>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>
#include <iostream>

struct JSONObject;

using JSONList = std::vector<JSONObject>;
using JSONDict = std::unordered_map<std::string, JSONObject>;

struct JSONObject {
    std::variant<
        std::monostate,
        bool,
        int,
        double,
        std::string,
        JSONList,
        JSONDict
    > inner;

    template<class T>
    bool is() const {
        return std::holds_alternative<T>(inner);
    }

    template<class T>
    T& get() {
        return std::get<T>(inner);
    }

    template<class T>
    const T& get() const {
        return std::get<T>(inner);
    }
};

template <class T>
std::optional<T> try_parse_num(std::string_view str) {
    T value;
    auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
        return value;
    }
    return std::nullopt;
}

char unescaped_char(char c) {
    switch (c) {
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'b': return '\b';
        case '0': return '\0';
        default:  return c;
    }
}

std::pair<JSONObject, size_t> parse(std::string_view json) {
    if (json.empty()) {
        return {{std::monostate{}}, 0};
    }
    if (size_t off = json.find_first_not_of(" \n\r\t\0"); off != 0 && off != json.npos) {
        auto [obj, eaten] = parse(json.substr(off));
        return {{std::move(obj)}, eaten + off};
    }
    if ('0' <= json[0] && json[0] <= '9' || json[0] == '-' || json[0] == '+') {
        std::regex num_re{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
        std::cmatch match;
        if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) {
            std::string str = match.str();
            if (auto num = try_parse_num<int>(str)) {
                return {{*num}, str.size()};
            }
            if (auto num = try_parse_num<double>(str)) {
                return {{*num}, str.size()};
            }
        }
    }
    if (json[0] == '"') {
        std::string res;
        size_t size = 1;
        enum {
            Raw,
            Escape,
        } phase = Raw;
        for (auto c : json.substr(1)) {
            size ++;
            if (phase == Raw) {
                if (c == '\\') {
                    phase = Escape;
                } else if (c == '"') {
                    size ++;
                    break;
                } else {
                    res += c;
                }
            } else if (phase == Escape) {
                res += unescaped_char(c);
                phase = Raw;
            }
        }
        return {{std::move(res)}, size};
    }
    if (json[0] == '[') {
        JSONList res;
        size_t size = 1;
        for (; size < json.size();) {
            if (json[size] == ']') {
                size ++;
                break;
            }

            auto [obj, eaten] = parse(json.substr(size));
            if (eaten == 0) {
                size = 0;
                break;
            }
            size += eaten;
            res.push_back(std::move(obj));

            if (json[size] == ',') {
                size ++;
            }
        }
        return {{std::move(res)}, size};
    }
    if (json[0] == '{') {
        JSONDict res;
        size_t size = 1;
        for (; size < json.size();) {
            if (json[size] == '}') {
                size ++;
                break;
            }
            auto [keyobj, keyeaten] = parse(json.substr(size));
            if (keyeaten == 0 || !std::holds_alternative<std::string>(keyobj.inner)) {
                size = 0;
                break;
            }
            size += keyeaten;

            if (json[size] == ':') {
                size ++;
            }

            auto [valobj, valeaten] = parse(json.substr(size));
            if (valeaten == 0) {
                size = 0;
                break;
            }
            size += valeaten;
            auto key = std::move(std::get<std::string>(keyobj.inner));
            res.try_emplace(std::move(key), std::move(valobj));

            if (json[size] == ',') {
                size ++;
            }
        }
        return {{std::move(res)}, size};
    }
    return {{std::monostate{}}, 0};
}
void visitJSON (const JSONObject& json) {
    std::visit([&] (const auto &obj) {
        if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, std::monostate>) {
            std::cout << "null";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, bool>) {
            std::cout << (obj ? "true" : "false");
        } else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, int>) {
            std::cout << obj;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, double>) {
            std::cout << obj;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, std::string>) {
            std::cout << '"' << obj << '"';
        } else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, JSONList>) {
            std::cout << "[";
            for (const auto& elem : obj) {
                visitJSON(elem);
                std::cout << ", ";
            }
            std::cout << "]";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, JSONDict>) {
            std::cout << "{";
            for (const auto& [key, val] : obj) {
                std::cout << '"' << key << "\": ";
                visitJSON(val);
                std::cout << ", ";
            }
            std::cout << "}";
        }
    }, json.inner);
}

int main() {
    std::string_view str = R"JSON({"a": [1, "11", 3.14], "b": { "a": 1, "b": 2}})JSON";
    auto [json, eaten] = parse(str);
    visitJSON(json);
}
