# JSONCPP

`JSONCPP` is a lightweight implementation of `JSON` Data Interchange Standard for `C++` programming language.

It is an ideal candidate to use with microcontrollers. The library was tested with [`Raspberry Pi Pico and Pico W`](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html).

`JSON` + `C++` = ❤️

## Add dependency

### CMake

`CMake` provides a convenient way to depend on the library.

```cmake
include(FetchContent)

FetchContent_Declare(
  JSONCPP
  GIT_REPOSITORY git@github.com:st235/JSONCPP.git
  GIT_TAG "main"
  GIT_SHALLOW TRUE
  GIT_PROGRESS ON
)
FetchContent_MakeAvailable(JSONCPP)

target_link_libraries(your-project-target jsoncpp)
```

Check out [`samples`](./samples/) for more.

## Exploring the API

Almost everything you might expect has been implemented. Take a look at this real-life example:

```cpp
  if (response.isObject() && response["results"].isArray()) {
      const auto& results_array = response["results"].asArray();

      for (size_t i = 0; i < results_array.size(); i++) {
          const auto& raw_character = results_array[i];

          Character c = {
              uint32_t(raw_character["id"].asNumber()),
              raw_character["name"].asString(),
              raw_character["species"].asString(),
              raw_character["gender"].asString(),
              raw_character["image"].asString()
          };

          ...
      }
  }
```

Here are a few more examples of the most common use cases for your reference.

### Create `Json` object from a `string`

```cpp
std::string json_text = ...
const auto& json = json::Json::fromJson(json_text);
```

### Declare `Json` object and dump it to back to `string`

```cpp
json::Json json = { 
  std::make_pair("a", json::Json({ json::Json(true), json::Json("b") })),
  std::make_pair("b", json::Json(129.1))
};

std::string json_text = json::Json::toString(json);
```

In this example, the `json object` will be _minified_. If you want to _beautify_ the JSON (i.e., make it human-readable), the library offers a default implementation of `JsonBeautifier`, which you can find in [the samples folder.](./samples/)

### Types

The following paragraphs provide a deep dive into the implementation details of the library.

JSON specification declares 4 types and 3 literals:
- Literals
    - `null`
    - `true`
    - `false`
- Types
    - `number`
    - `string`
    - `array`
    - `object`

All json values are conform to specially defined [`json::Json` type](./include/json.h).
To check for those literals and types `Json` defines special **boolean methods**:

- `Json#isNull`
- `Json#isBool`
- `Json#isNumber`
- `Json#isString`
- `Json#isArray`
- `Json#isObject`

Almost all `is` methods (except `isNull`) has corresponding `as` methods to safely get the content of json file:

- `asBool` -> returns `bool`
- `asNumber` -> `double`
- `asString` -> `std::string`
- `asArray` -> `std::vector<Json>`
- `asObject` -> `std::unordered_map<std::string, Json>`

## Json Grammar Rules

The specification of JSON format is available at [_the oficial website_](https://www.json.org/json-en.html) or as [_ECMA-404 The JSON Data Interchange Standard_](https://ecma-international.org/publications-and-standards/standards/ecma-404/).

The grammar specifies 6 entries: `array`, `number`, `object`, `string`, `value`, and `whitespace`.
`value` is the entry point.

I won't cover all the details of the implemention but will provide the most important grammar rules.

| Array | Number | Object |
| ----- | ----- | ----- |
| ![Array](./images/array.png) | ![Number](./images/number.png) | ![Object](./images/object.png) |

| String | Value | Whitespace |
| ----- | ----- | ----- |
| ![String](./images/string.png) | ![Value](./images/value.png) | ![Whitespace](./images/whitespace.png) |

## Contribution

The project is using `CMake` as the build system.

### Building

Use these commands to build the project:

```bash
mkdir build
cmake .. -DCOMPILE_TESTS=ON -DENABLE_ASSERT=ON
make
```
>[!TIP]
> When `ENABLE_ASSERT=ON` is provided, the assertion mechanism is turned on.
> While this can be useful during debugging, usually, you don't want them enabled in the production flavour.

### Running test

A lot of logic in the library heavily relies on _unit_ and _integration_ tests.
To run them you need yo _successfully build the project_ and run the command below:

```bash
ctest --output-on-failure
```
