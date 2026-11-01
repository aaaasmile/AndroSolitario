#include "Json.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>

namespace invjson {
namespace {

const char kHexDigits[] = "0123456789abcdef";
const int kMaximumNestingDepth = 128;

void AppendUtf8(std::string& output, unsigned int codePoint) {
    if (codePoint <= 0x7f) {
        output.push_back(static_cast<char>(codePoint));
    } else if (codePoint <= 0x7ff) {
        output.push_back(static_cast<char>(0xc0 | (codePoint >> 6)));
        output.push_back(static_cast<char>(0x80 | (codePoint & 0x3f)));
    } else if (codePoint <= 0xffff) {
        output.push_back(static_cast<char>(0xe0 | (codePoint >> 12)));
        output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f)));
        output.push_back(static_cast<char>(0x80 | (codePoint & 0x3f)));
    } else {
        output.push_back(static_cast<char>(0xf0 | (codePoint >> 18)));
        output.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3f)));
        output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f)));
        output.push_back(static_cast<char>(0x80 | (codePoint & 0x3f)));
    }
}

class Parser {
   public:
    explicit Parser(const std::string& text)
        : _text(text), _offset(0), _depth(0), _failed(false), _errorOffset(0) {}

    ParseResult Run() {
        SkipWhitespace();
        Value value = ParseValue();
        SkipWhitespace();
        if (!_failed && _offset != _text.size()) {
            Fail("Unexpected trailing characters");
        }

        ParseResult result;
        result.value = value;
        result.success = !_failed;
        result.errorOffset = _errorOffset;
        result.errorMessage = _errorMessage;
        return result;
    }

   private:
    Value ParseValue() {
        if (_offset >= _text.size()) {
            Fail("Expected a JSON value");
            return Value();
        }

        switch (_text[_offset]) {
            case 'n':
                return ParseLiteral("null", Value());
            case 't':
                return ParseLiteral("true", Value(true));
            case 'f':
                return ParseLiteral("false", Value(false));
            case '"':
                return Value(ParseString());
            case '[':
                return ParseArray();
            case '{':
                return ParseObject();
            default:
                if (_text[_offset] == '-' || IsDigit(_text[_offset])) {
                    return ParseNumber();
                }
                Fail("Unexpected character while parsing a value");
                return Value();
        }
    }

    Value ParseLiteral(const char* literal, const Value& value) {
        const std::string expected(literal);
        if (_text.compare(_offset, expected.size(), expected) != 0) {
            Fail("Invalid JSON literal");
            return Value();
        }
        _offset += expected.size();
        return value;
    }

    Value ParseArray() {
        if (!EnterContainer()) {
            return Value::MakeArray();
        }

        Value result = Value::MakeArray();
        ++_offset;
        SkipWhitespace();
        if (Consume(']')) {
            --_depth;
            return result;
        }

        while (!_failed) {
            result.PushBack(ParseValue());
            SkipWhitespace();
            if (Consume(']')) {
                break;
            }
            if (!Consume(',')) {
                Fail("Expected ',' or ']' in array");
                break;
            }
            SkipWhitespace();
        }
        --_depth;
        return result;
    }

    Value ParseObject() {
        if (!EnterContainer()) {
            return Value::MakeObject();
        }

        Value result = Value::MakeObject();
        ++_offset;
        SkipWhitespace();
        if (Consume('}')) {
            --_depth;
            return result;
        }

        while (!_failed) {
            if (_offset >= _text.size() || _text[_offset] != '"') {
                Fail("Expected a quoted object key");
                break;
            }
            const std::string key = ParseString();
            SkipWhitespace();
            if (!Consume(':')) {
                Fail("Expected ':' after object key");
                break;
            }
            SkipWhitespace();
            result[key] = ParseValue();
            SkipWhitespace();
            if (Consume('}')) {
                break;
            }
            if (!Consume(',')) {
                Fail("Expected ',' or '}' in object");
                break;
            }
            SkipWhitespace();
        }
        --_depth;
        return result;
    }

    std::string ParseString() {
        std::string result;
        ++_offset;
        while (_offset < _text.size() && !_failed) {
            const unsigned char ch = static_cast<unsigned char>(_text[_offset++]);
            if (ch == '"') {
                return result;
            }
            if (ch < 0x20) {
                Fail("Unescaped control character in string");
                return result;
            }
            if (ch != '\\') {
                result.push_back(static_cast<char>(ch));
                continue;
            }
            if (_offset >= _text.size()) {
                Fail("Incomplete escape sequence");
                return result;
            }

            const char escape = _text[_offset++];
            switch (escape) {
                case '"':
                case '\\':
                case '/':
                    result.push_back(escape);
                    break;
                case 'b':
                    result.push_back('\b');
                    break;
                case 'f':
                    result.push_back('\f');
                    break;
                case 'n':
                    result.push_back('\n');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                case 'u':
                    AppendUnicodeEscape(result);
                    break;
                default:
                    Fail("Invalid string escape");
                    break;
            }
        }
        if (!_failed) {
            Fail("Unterminated string");
        }
        return result;
    }

    void AppendUnicodeEscape(std::string& output) {
        unsigned int codePoint = 0;
        if (!ReadHexQuad(codePoint)) {
            return;
        }
        if (codePoint >= 0xd800 && codePoint <= 0xdbff) {
            if (_offset + 2 > _text.size() || _text[_offset] != '\\' ||
                _text[_offset + 1] != 'u') {
                Fail("High surrogate must be followed by a low surrogate");
                return;
            }
            _offset += 2;
            unsigned int lowSurrogate = 0;
            if (!ReadHexQuad(lowSurrogate)) {
                return;
            }
            if (lowSurrogate < 0xdc00 || lowSurrogate > 0xdfff) {
                Fail("Invalid low surrogate");
                return;
            }
            codePoint = 0x10000 + ((codePoint - 0xd800) << 10) +
                        (lowSurrogate - 0xdc00);
        } else if (codePoint >= 0xdc00 && codePoint <= 0xdfff) {
            Fail("Unexpected low surrogate");
            return;
        }
        AppendUtf8(output, codePoint);
    }

    bool ReadHexQuad(unsigned int& value) {
        if (_offset + 4 > _text.size()) {
            Fail("Incomplete Unicode escape");
            return false;
        }
        value = 0;
        for (int i = 0; i < 4; ++i) {
            const char digit = _text[_offset++];
            value <<= 4;
            if (digit >= '0' && digit <= '9') {
                value |= static_cast<unsigned int>(digit - '0');
            } else if (digit >= 'a' && digit <= 'f') {
                value |= static_cast<unsigned int>(digit - 'a' + 10);
            } else if (digit >= 'A' && digit <= 'F') {
                value |= static_cast<unsigned int>(digit - 'A' + 10);
            } else {
                Fail("Invalid Unicode escape");
                return false;
            }
        }
        return true;
    }

    Value ParseNumber() {
        const size_t start = _offset;
        Consume('-');
        if (Consume('0')) {
            if (_offset < _text.size() && IsDigit(_text[_offset])) {
                Fail("Leading zeros are not allowed");
                return Value();
            }
        } else if (!ConsumeDigits()) {
            Fail("Expected digits in number");
            return Value();
        }

        if (Consume('.')) {
            if (!ConsumeDigits()) {
                Fail("Expected digits after decimal point");
                return Value();
            }
        }
        if (_offset < _text.size() && (_text[_offset] == 'e' || _text[_offset] == 'E')) {
            ++_offset;
            if (_offset < _text.size() && (_text[_offset] == '+' || _text[_offset] == '-')) {
                ++_offset;
            }
            if (!ConsumeDigits()) {
                Fail("Expected digits in exponent");
                return Value();
            }
        }

        const std::string numberText = _text.substr(start, _offset - start);
        char* end = NULL;
        const double number = std::strtod(numberText.c_str(), &end);
        if (end != numberText.c_str() + numberText.size() ||
            !std::isfinite(number)) {
            Fail("Number is outside the supported range");
            return Value();
        }
        return Value(number);
    }

    bool ConsumeDigits() {
        const size_t start = _offset;
        while (_offset < _text.size() && IsDigit(_text[_offset])) {
            ++_offset;
        }
        return _offset != start;
    }

    bool EnterContainer() {
        if (_depth >= kMaximumNestingDepth) {
            Fail("Maximum JSON nesting depth exceeded");
            return false;
        }
        ++_depth;
        return true;
    }

    void SkipWhitespace() {
        while (_offset < _text.size()) {
            const char ch = _text[_offset];
            if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
                break;
            }
            ++_offset;
        }
    }

    bool Consume(char expected) {
        if (_offset < _text.size() && _text[_offset] == expected) {
            ++_offset;
            return true;
        }
        return false;
    }

    void Fail(const std::string& message) {
        if (_failed) {
            return;
        }
        _failed = true;
        _errorOffset = _offset;
        _errorMessage = message;
    }

    static bool IsDigit(char ch) { return ch >= '0' && ch <= '9'; }

    const std::string& _text;
    size_t _offset;
    int _depth;
    bool _failed;
    size_t _errorOffset;
    std::string _errorMessage;
};

void AppendIndent(std::string& output, int depth) {
    output.append(static_cast<size_t>(depth * 2), ' ');
}

void AppendEscapedString(std::string& output, const std::string& value) {
    output.push_back('"');
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it) {
        const unsigned char ch = static_cast<unsigned char>(*it);
        switch (ch) {
            case '"':
                output += "\\\"";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            default:
                if (ch < 0x20) {
                    output += "\\u00";
                    output.push_back(kHexDigits[ch >> 4]);
                    output.push_back(kHexDigits[ch & 0x0f]);
                } else {
                    output.push_back(static_cast<char>(ch));
                }
                break;
        }
    }
    output.push_back('"');
}

void AppendValue(std::string& output, const Value& value, bool pretty,
                 int depth) {
    switch (value.GetType()) {
        case Value::Type::Null:
            output += "null";
            return;
        case Value::Type::Boolean:
            output += value.AsBoolean() ? "true" : "false";
            return;
        case Value::Type::Number: {
            if (!std::isfinite(value.AsNumber())) {
                output += "null";
                return;
            }
            std::ostringstream stream;
            stream << std::setprecision(std::numeric_limits<double>::max_digits10)
                   << value.AsNumber();
            output += stream.str();
            return;
        }
        case Value::Type::String:
            AppendEscapedString(output, value.AsString());
            return;
        case Value::Type::Array: {
            const Value::Array& array = value.AsArray();
            output.push_back('[');
            for (size_t i = 0; i < array.size(); ++i) {
                if (i != 0) {
                    output.push_back(',');
                }
                if (pretty) {
                    output.push_back('\n');
                    AppendIndent(output, depth + 1);
                }
                AppendValue(output, array[i], pretty, depth + 1);
            }
            if (pretty && !array.empty()) {
                output.push_back('\n');
                AppendIndent(output, depth);
            }
            output.push_back(']');
            return;
        }
        case Value::Type::Object: {
            const Value::Object& object = value.AsObject();
            output.push_back('{');
            Value::Object::const_iterator it = object.begin();
            for (size_t i = 0; it != object.end(); ++it, ++i) {
                if (i != 0) {
                    output.push_back(',');
                }
                if (pretty) {
                    output.push_back('\n');
                    AppendIndent(output, depth + 1);
                }
                AppendEscapedString(output, it->first);
                output += pretty ? ": " : ":";
                AppendValue(output, it->second, pretty, depth + 1);
            }
            if (pretty && !object.empty()) {
                output.push_back('\n');
                AppendIndent(output, depth);
            }
            output.push_back('}');
            return;
        }
    }
}

}  // namespace

Value::Value() : _type(Type::Null), _boolean(false), _number(0.0) {}
Value::Value(bool value)
    : _type(Type::Boolean), _boolean(value), _number(0.0) {}
Value::Value(double value)
    : _type(Type::Number), _boolean(false), _number(value) {}
Value::Value(int value)
    : _type(Type::Number), _boolean(false), _number(value) {}
Value::Value(const char* value)
    : _type(Type::String), _boolean(false), _number(0.0), _string(value ? value : "") {}
Value::Value(const std::string& value)
    : _type(Type::String), _boolean(false), _number(0.0), _string(value) {}

Value Value::MakeArray() {
    Value value;
    value._type = Type::Array;
    return value;
}

Value Value::MakeObject() {
    Value value;
    value._type = Type::Object;
    return value;
}

Value::Type Value::GetType() const { return _type; }
bool Value::IsNull() const { return _type == Type::Null; }
bool Value::AsBoolean() const { return _boolean; }
double Value::AsNumber() const { return _number; }
const std::string& Value::AsString() const { return _string; }
const Value::Array& Value::AsArray() const { return _array; }
Value::Array& Value::AsArray() { return _array; }
const Value::Object& Value::AsObject() const { return _object; }
Value::Object& Value::AsObject() { return _object; }

void Value::PushBack(const Value& value) {
    if (_type == Type::Array) {
        _array.push_back(value);
    }
}

Value& Value::operator[](const std::string& key) {
    if (_type != Type::Object) {
        _type = Type::Object;
        _array.clear();
        _object.clear();
        _string.clear();
        _boolean = false;
        _number = 0.0;
    }
    return _object[key];
}

const Value* Value::Find(const std::string& key) const {
    if (_type != Type::Object) {
        return NULL;
    }
    Object::const_iterator it = _object.find(key);
    return it == _object.end() ? NULL : &it->second;
}

ParseResult::ParseResult()
    : success(false), errorOffset(0) {}

ParseResult Parse(const std::string& json) { return Parser(json).Run(); }

std::string Stringify(const Value& value, bool pretty) {
    std::string result;
    AppendValue(result, value, pretty, 0);
    return result;
}

}  // namespace invjson
