#ifndef INV_JSON_H
#define INV_JSON_H

#include <map>
#include <string>
#include <vector>

namespace invjson {

class Value {
   public:
    enum class Type { Null, Boolean, Number, String, Array, Object };
    typedef std::vector<Value> Array;
    typedef std::map<std::string, Value> Object;

    Value();
    explicit Value(bool value);
    explicit Value(double value);
    explicit Value(int value);
    explicit Value(const char* value);
    explicit Value(const std::string& value);

    static Value MakeArray();
    static Value MakeObject();

    Type GetType() const;
    bool IsNull() const;
    bool AsBoolean() const;
    double AsNumber() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    Array& AsArray();
    const Object& AsObject() const;
    Object& AsObject();

    void PushBack(const Value& value);
    Value& operator[](const std::string& key);
    const Value* Find(const std::string& key) const;

   private:
    Type _type;
    bool _boolean;
    double _number;
    std::string _string;
    Array _array;
    Object _object;
};

struct ParseResult {
    ParseResult();

    Value value;
    bool success;
    size_t errorOffset;
    std::string errorMessage;
};

ParseResult Parse(const std::string& json);
std::string Stringify(const Value& value, bool pretty = true);

}  // namespace invjson

#endif
