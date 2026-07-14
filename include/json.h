#ifndef JSONCPP_JSON_H_
#define JSONCPP_JSON_H_

#include <initializer_list>
#include <optional>
#include <unordered_set>
#include <utility>

#include "json_assert.h"
#include "json_definitions.h"
#include "json_visitor.h"

namespace json {

class Json {
  private:
    struct ValueContainer {
      public:
        enum {
          kTypeNull,
          kTypeBool,
          kTypeNumber,
          kTypeString,
          kTypeArray,
          kTypeObject,
        } type;

        union {
          bool_t _bool;
          number_t _number;
          string_t* _string;
          array_t* _array;
          object_t* _object;
        } value;

        ValueContainer() noexcept:
          type(kTypeNull) {
            value._object = nullptr;
        }

        explicit ValueContainer(bool_t raw_value) noexcept:
          type(kTypeBool) {
            value._bool = raw_value;
        }

        explicit ValueContainer(number_t raw_value) noexcept:
          type(kTypeNumber) {
            value._number = raw_value;
        }

        ValueContainer(const string_t& raw_value) noexcept:
          type(kTypeString) {
            value._string = new string_t(raw_value);
        }

        ValueContainer(string_t&& raw_value) noexcept:
          type(kTypeString) {
            value._string = new string_t(std::move(raw_value));
        }

        ValueContainer(const array_t& raw_value) noexcept:
          type(kTypeArray) {
            value._array = new array_t(raw_value);
        }

        ValueContainer(array_t&& raw_value) noexcept:
          type(kTypeArray) {
            value._array = new array_t(std::move(raw_value));
        }

        ValueContainer(const object_t& raw_value) noexcept:
          type(kTypeObject) {
            value._object = new object_t(raw_value);
        }

        ValueContainer(object_t&& raw_value) noexcept:
          type(kTypeObject) {
            value._object = new object_t(std::move(raw_value));
        }

        ValueContainer(const ValueContainer& that) noexcept:
          type(that.type) {
            copyContainerValue(that);
        }

        ValueContainer& operator=(const ValueContainer& that) noexcept {
          if (this != &that) {
            // Delete old value.
            deleteContainerValue();

            type = that.type;
            copyContainerValue(that);
          }

          return *this;
        }

        ValueContainer(ValueContainer&& that) noexcept:
          type(that.type) {
            swapContainerValue(std::move(that));
        }

        ValueContainer& operator=(ValueContainer&& that) noexcept {
          if (this != &that) {
            // Delete old value.
            deleteContainerValue();

            type = that.type;
            swapContainerValue(std::move(that));
          }

          return *this;
        }

        ~ValueContainer() {
          deleteContainerValue();
        }

      private:
        void copyContainerValue(const ValueContainer& that) {
          switch (that.type) {
            case kTypeNull:
              value._object = nullptr;
              break;
            case kTypeBool:
              value._bool = that.value._bool;
              break;
            case kTypeNumber:
              value._number = that.value._number;
              break;
            case kTypeString:
              value._string = new string_t(*that.value._string);
              break;
            case kTypeArray:
              value._array = new array_t(*that.value._array);
              break;
            case kTypeObject:
              value._object = new object_t(*that.value._object);
              break;
          }
        }

        void swapContainerValue(ValueContainer&& that) {
          switch (that.type) {
            case kTypeNull:
              value._object = nullptr;
              break;
            case kTypeBool:
              value._bool = that.value._bool;
              break;
            case kTypeNumber:
              value._number = that.value._number;
              break;
            case kTypeString:
              value._string = new string_t(std::move(*that.value._string));
              break;
            case kTypeArray:
              value._array = new array_t(std::move(*that.value._array));
              break;
            case kTypeObject:
              value._object = new object_t(std::move(*that.value._object));
              break;
          }
        }

        void deleteContainerValue() {
          switch (type) {
            case kTypeNull:
            case kTypeBool:
            case kTypeNumber:
              break;
            case kTypeString:
              delete value._string;
              break;
            case kTypeArray:
              delete value._array;
              break;
            case kTypeObject:
              delete value._object;
              break;
          }
        }
    };

  static const Json VALUE_NULL;

  ValueContainer _container;

  public:
    static const Json& null();
    static Json array();
    static Json object();

    static std::optional<Json> fromJson(const std::string& json);
    static std::string toJson(const Json& json);

    Json() noexcept: _container() {}

    Json(bool_t value) noexcept: _container(value) {}

    Json(number_t value) noexcept: _container(value) {}

    Json(const string_t& value) noexcept: _container(value) {}
    Json(string_t&& value) noexcept: _container(std::move(value)) {}
    Json(const char* value) noexcept: _container(std::string(value)) {}

    Json(const array_t& value) noexcept: _container(value) {}
    Json(array_t&& value) noexcept: _container(std::move(value)) {}
    Json(std::initializer_list<Json> init): _container(array_t(init)) {}

    Json(const object_t& value) noexcept: _container(value) {}
    Json(object_t&& value) noexcept: _container(std::move(value)) {}
    Json(std::initializer_list<std::pair<std::string, Json>> init): _container(object_t()) {
      for (const auto& [key, value]: init) {
        (*_container.value._object)[key] = value;
      }
    }

    Json(const Json& that) noexcept:
      _container(that._container) {
    }

    Json& operator=(const Json& that) noexcept {
      if (this != &that) {
        _container = that._container;
      }
      return *this;
    }

    Json(Json&& that) noexcept:
      _container(std::move(that._container)) {
    }

    Json& operator=(Json&& that) noexcept {
      if (this != &that) {
        _container = std::move(that._container);
      }
      return *this;
    }

    bool operator==(const Json& that) const {
      if (_container.type != that._container.type) {
        return false;
      }

      switch (_container.type) {
        case ValueContainer::kTypeNull:
          return _container.value._object == that._container.value._object;
        case ValueContainer::kTypeBool:
          return _container.value._bool == that._container.value._bool;
        case ValueContainer::kTypeNumber:
          return _container.value._number == that._container.value._number;
        case ValueContainer::kTypeString:
          return *_container.value._string == *that._container.value._string;
        case ValueContainer::kTypeArray:
          return *_container.value._array == *that._container.value._array;
        case ValueContainer::kTypeObject:
          return *_container.value._object == *that._container.value._object;
        default:
          JUNREACHABLE();
          return false;
      }
    }

    bool operator!=(const Json& that) const {
      return !operator==(that);
    }

    explicit operator bool() const {
      switch (_container.type) {
        case ValueContainer::kTypeNull:
          return false;
        case ValueContainer::kTypeBool:
          return _container.value._bool != false;
        case ValueContainer::kTypeNumber:
          return _container.value._number != 0.0;
        case ValueContainer::kTypeString:
          return _container.value._string->length() > 0;
        case ValueContainer::kTypeArray:
          return _container.value._array->size() > 0;
        case ValueContainer::kTypeObject:
          return _container.value._object->size() > 0;
        default:
          JUNREACHABLE();
          return false;
      }
    }

    inline bool isNull() const {
      return _container.type == ValueContainer::kTypeNull;
    }

    inline bool isBool() const {
      return _container.type == ValueContainer::kTypeBool;
    }

    inline bool isNumber() const {
      return _container.type == ValueContainer::kTypeNumber;
    }

    inline bool isString() const {
      return _container.type == ValueContainer::kTypeString;
    }

    inline bool isArray() const {
      return _container.type == ValueContainer::kTypeArray;
    }

    inline bool isObject() const {
      return _container.type == ValueContainer::kTypeObject;
    }

    inline bool_t asBool() const {
      JASSERT(isBool());
      return _container.value._bool;
    }

    inline number_t asNumber() const {
      JASSERT(isNumber());
      return _container.value._number;
    }

    inline const string_t& asString() const {
      JASSERT(isString());
      return *_container.value._string;
    }

    inline const array_t& asArray() const {
      JASSERT(isArray());
      return *_container.value._array;
    }

    inline const object_t& asObject() const {
      JASSERT(isObject());
      return *_container.value._object;
    }

    Json& operator[](size_t index) {
      JASSERT(isArray());
      return (*_container.value._array)[index];
    }

    const Json& operator[](size_t index) const {
      if (!isArray()) {
        JTHROW();
        return Json::null();
      }
      return (*_container.value._array)[index];
    }

    Json& operator[](const std::string& key) {
      JASSERT(isObject());
      return (*_container.value._object)[key];
    }

    const Json& operator[](const std::string& key) const {
      if (!isObject()) {
        JTHROW();
        return Json::null();
      }

      if (!containsKey(key)) {
        return Json::null();
      }

      return (*_container.value._object)[key];
    }

    bool containsKey(const std::string& key) const {
      if (!isObject()) {
        JTHROW();
        return false;
      }

      const object_t& content_object = *_container.value._object;
      return content_object.find(key) != content_object.end();
    }

    std::unordered_set<std::string> keys() const {
      if (!isObject()) {
        JTHROW();
        return std::unordered_set<std::string>();
      }

      std::unordered_set<std::string> keys;
      for (const auto& [key, _]: (*_container.value._object)) {
        keys.insert(key);
      }

      return keys;
    }

    bool add(const Json& that) {
      if (!isArray()) {
        JTHROW();
        return false;
      }

      _container.value._array->push_back(that);
      return true;
    }

    bool add(Json&& that) {
      if (!isArray()) {
        JTHROW();
        return false;
      }

      _container.value._array->emplace_back(std::move(that));
      return true;
    }

    bool put(const std::string& key, const Json& json) {
      if (!isObject()) {
        JTHROW();
        return false;
      }

      (*_container.value._object)[key] = json;
      return true;
    }

    bool put(const std::string& key, Json&& json) {
      if (!isObject()) {
        JTHROW();
        return false;
      }

      (*_container.value._object)[key] = std::move(json);
      return true;
    }

    size_t size() const {
      JASSERT(isArray() || isObject());

      switch (_container.type) {
        case ValueContainer::kTypeArray:
          return _container.value._array->size();
        case ValueContainer::kTypeObject:
          return _container.value._object->size();
        default:
          JUNREACHABLE();
          return 0;
      }
    }

    void accept(JsonVisitor& visitor) const {
      switch (_container.type) {
        case ValueContainer::kTypeNull:
          visitor.visitNull();
          break;
        case ValueContainer::kTypeBool:
          visitor.visitBoolean(asBool());
          break;
        case ValueContainer::kTypeNumber:
          visitor.visitNumber(asNumber());
          break;
        case ValueContainer::kTypeString:
          visitor.visitString(asString());
          break;
        case ValueContainer::kTypeArray:
          visitor.visitArray(asArray());
          break;
        case ValueContainer::kTypeObject:
          visitor.visitObject(asObject());
          break;
      }
    }

    ~Json() = default;
};

} // namespace json

#endif // JSONCPP_JSON_H_
