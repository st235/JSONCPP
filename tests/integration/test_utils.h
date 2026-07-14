#ifndef JSONCPP_TEST_UTILS_H_
#define JSONCPP_TEST_UTILS_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace test_utils {

// Expected value is a vector of strings as JSON specification
// does not provide any gurantees on how keys are ordered within a
// JSON Object, therefore we provide all permutations of keys and
// expect that one of them matches.
template<typename T>
using TestData = std::pair<T, std::vector<std::string>>;

template<typename T>
inline TestData<T> PrepareTestData(const T& actual,
                                   const std::vector<std::string>& expected) {
    return std::make_pair(actual, expected);
}

} // namespace test_utils

#endif // JSONCPP_TEST_UTILS_H_
