#include "json.h"

#include "json_parser.h"
#include "json_minifier.h"

namespace json {

const Json Json::VALUE_NULL = Json();

const Json& Json::null() {
    return VALUE_NULL;
}

Json Json::array() {
    return Json(array_t());
}

Json Json::object() {
    return Json(object_t());
}

std::optional<Json> Json::fromJson(const std::string& json) {
    __internal::JsonParser parser(json);
    return parser.Parse();
}

std::string Json::toJson(const Json& json) {
    JsonMinifier minifier;
    return minifier.minify(json);
}

} // namespace json
