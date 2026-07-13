#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "json.h"

namespace {

struct Character {
    uint32_t id;
    std::string name;
    std::string species;
    std::string gender;
    std::string image;
};

std::vector<Character> ParseCharacters(const json::Json& response) {
    std::vector<Character> result;

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

            result.push_back(c);
        }
    }

    return result;
}

std::string ReadFile(const std::string& filename) {
    std::ifstream file_stream(filename);
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

} // namespace

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Rick And Morty API parser expects a single json file,"  << std::endl
                  << "See the command below:" << std::endl
                  << "\tresponse-parser [api-response.json]" << std::endl;
        return 0;
    }

    const auto& file_context = ReadFile(std::string(argv[1]));

    const auto& json = json::Json::fromJson(file_context);
    if (!json) {
        std::cout << "Invalid json."  << std::endl;
        return 0;
    }

    const auto& characters = ParseCharacters(*json);
    if (characters.empty()) {
        std::cout << "Response is empty?"  << std::endl;
        return 0;
    }

    std::cout << "Parsed characters: " << std::endl;

    for (const auto& character: characters) {
        std::cout << "id=" << character.id 
                  << " name=" << character.name
                  << " species=" << character.species
                  << " gender=" << character.gender
                  << " image=" << character.image << std::endl;
    }

    return 0;
}
