#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <print>
#include <regex>
#include <streambuf>
#include <string>

/** @brief The pattern for matching all valid `mul` instructions in memory. */
static const std::regex ALL_PATTERN(
    R"(mul\((\d{1,3}),(\d{1,3})\))",
    std::regex_constants::ECMAScript | std::regex_constants::optimize
);

/** @brief The pattern for matching enabled `mul` instructions in memory. */
static const std::regex ENABLED_PATTERN(
    R"((?:do\(\)|don't\(\)|mul\((\d{1,3}),(\d{1,3})\)))",
    std::regex_constants::ECMAScript | std::regex_constants::optimize
);

/**
 * @brief Returns the sum of the results of all valid `mul` instructions in the
 * input.
 *
 * The input is expected to consist of zero or more lines of text, each of which
 * may contain zero or more valid `mul` instructions. A valid `mul` instruction
 * is defined as a substring of the form `mul(X,Y)`, where `X` and `Y` are
 * integers between `0` and `999` (inclusive).
 *
 * An example for an input might be the following:
 *
 * ```plaintext
 * xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))
 * ```
 *
 * @param[in] memory The input to scan.
 * @return The sum of the results of all valid `mul` instructions in the input.
 */
static inline std::int64_t sum_of_all_muls(std::string_view memory) {
    std::int64_t sum = 0;
    std::regex_iterator<std::string_view::iterator> it(
        memory.begin(),
        memory.end(),
        ALL_PATTERN
    );
    for (std::regex_iterator<std::string_view::iterator> end; it != end; it++) {
        const std::cmatch& match = *it;
        sum += std::stoll(match[1]) * std::stoll(match[2]);
    }
    return sum;
}

/**
 * @brief Returns the sum of the results of all enabled `mul` instructions in
 * the input.
 *
 * The input is expected to consist of zero or more lines of text, each of which
 * may contain zero or more valid `mul` instructions, as well as `do()` and
 * `don't()` instructions. A valid `mul` instruction is defined as a substring
 * of the form `mul(X,Y)`, where `X` and `Y` are integers between `0` and
 * `999` (inclusive). An enabled `mul` instruction is a valid `mul` instruction
 * that is preceded by a `do()` instruction, but not by a `don't()` instruction.
 *
 * An example for an input might be the following:
 *
 * ```plaintext
 * do()mul(2,4)don't()mul(3,7)do()mul(32,64)then(mul(11,8)mul(8,5))
 * ```
 *
 * @param[in] memory The input to scan.
 * @return The sum of the results of all enabled `mul` instructions in the
 * input.
 */
static inline std::int64_t sum_of_enabled_muls(std::string_view memory) {
    std::int64_t sum = 0;
    bool enabled = true;
    std::regex_iterator<std::string_view::iterator> it(
        memory.begin(),
        memory.end(),
        ENABLED_PATTERN
    );
    for (std::regex_iterator<std::string_view::iterator> end; it != end; it++) {
        const std::cmatch& match = *it;
        if (match[0] == "do()") {
            enabled = true;
        }
        else if (match[0] == "don't()") {
            enabled = false;
        }
        else if (enabled) {
            sum += std::stoll(match[1]) * std::stoll(match[2]);
        }
    }
    return sum;
}

int main() {
    std::string input(
        std::istreambuf_iterator<char>(std::cin),
        std::istreambuf_iterator<char>()
    );
    std::int64_t sumAll = sum_of_all_muls(input);
    std::int64_t sumEnabled = sum_of_enabled_muls(input);
    std::println("The sum of all multiplications is {}.", sumAll);
    std::println("The sum of enabled multiplications is {}.", sumEnabled);
    return EXIT_SUCCESS;
}