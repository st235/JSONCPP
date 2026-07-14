#ifndef JSONCPP_JSON_DEFINITIONS_H_
#define JSONCPP_JSON_DEFINITIONS_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace json {

class Json;

using bool_t = bool;
using number_t = double;
using string_t = std::string;
using array_t = std::vector<Json>;
using object_t = std::unordered_map<std::string, Json>;

} // namespace json

#endif // JSONCPP_JSON_DEFINITIONS_H_
