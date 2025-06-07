#ifndef TYPES_HPP
#define TYPES_HPP

namespace mylang {

enum class Type { Int, Float, String, Void };

inline const char* typeToString(Type t) {
    switch (t) {
        case Type::Int: return "int";
        case Type::Float: return "float";
        case Type::String: return "string";
        case Type::Void: return "void";
    }
    return "unknown";
}

} // namespace mylang

#endif // TYPES_HPP
