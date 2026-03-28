#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents an equation for a bridge repair. */
class Equation final {
private:

    /** @brief The test value of the equation. */
    i64 testValue;

    /** @brief The values of the equation. */
    std::vector<i64> values;

    /**
     * @brief Initializes a new equation with the specified test value and
     * values.
     *
     * @param[in] testValue The test value of the equation.
     * @param[in] values    The values of the equation.
     */
    Equation(i64 testValue, std::vector<i64> values)
        : testValue(testValue), values(std::move(values)) { }

    /**
     * @brief Determines whether this equation is solvable by recursively trying
     * all possible combinations of addition and multiplication of the values.
     *
     * @param[in] index  The current index.
     * @param[in] result The current result.
     * @return `true` if this equation is solvable, otherwise `false`.
     */
    bool is_solvable(isize index, i64 result) const noexcept {
        assert((index >= 0) && (index <= std::ssize(values)));
        assert(result >= 0);
        if (index == std::ssize(values)) {
            return result == testValue;
        }
        if (result > testValue) {
            return false;
        }
        return is_solvable(index + 1, result + values[index])
            || is_solvable(index + 1, result * values[index]);
    }

    /**
     * @brief Concatenates two positive integers by appending the digits of the
     * second integer to the first integer.
     *
     * @param[in] a The first positive integer.
     * @param[in] b The second positive integer.
     * @return The concatenation of the two integers.
     */
    static i64 concat(i64 a, i64 b) noexcept {
        assert(a >= 0);
        assert(b >= 0);
        i64 shift = 10;
        while (shift <= b) {
            shift *= 10;
        }
        return (a * shift) + b;
    }

    /**
     * @brief Determines whether this equation is solvable by recursively trying
     * all possible combinations of addition, multiplication, and concatenation
     * of the values.
     *
     * @param[in] index  The current index.
     * @param[in] result The current result.
     * @return `true` if this equation is solvable, otherwise `false`.
     */
    bool is_solvable_with_concat(isize index, i64 result) const noexcept {
        assert((index >= 0) && (index <= std::ssize(values)));
        assert(result >= 0);
        if (index == std::ssize(values)) {
            return result == testValue;
        }
        if (result > testValue) {
            return false;
        }
        return is_solvable_with_concat(index + 1, result + values[index])
            || is_solvable_with_concat(index + 1, result * values[index])
            || is_solvable_with_concat(index + 1, concat(result, values[index]));
    }

public:

    /**
     * @brief Parses an equation from a specified line of text.
     *
     * The line must have the following format:
     *
     * ```plaintext
     * <testValue>: <values>
     * ```
     *
     * Here, `<testValue>` is a positive integer representing the test value of
     * the equation, and `<values>` is a list of positive integers (separated by
     * spaces) representing the values of the equation.
     *
     * An example for a valid line of text might be the following:
     *
     * ```plaintext
     * 3267: 81 40 27
     * ```
     *
     * @param[in] line The line of text to parse.
     * @return The parsed equation on success, or `std::nullopt` on failure.
     */
    static std::optional<Equation> parse(const std::string& line) {
        i64 testValue;
        char separator;
        std::istringstream iss(line);
        if (
            !(iss >> testValue >> separator) || (testValue < 0)
                || (separator != ':')
        ) {
            return std::nullopt;
        }
        std::vector<i64> values;
        i64 value;
        while (iss >> value) {
            if (value < 0) {
                return std::nullopt;
            }
            values.push_back(value);
        }
        return Equation(testValue, std::move(values));
    }

    /**
     * @brief Determines whether this equation is solvable.
     *
     * An equation is considered solvable if there exists a combination of
     * addition and multiplication of the values that results in the test value.
     *
     * @return `true` if this equation is solvable, otherwise `false`.
     */
    bool is_solvable() const noexcept {
        return values.empty() ? (testValue == 0) : is_solvable(1, values[0]);
    }

    /**
     * @brief Determines whether this equation is solvable with concatenation.
     *
     * An equation is considered solvable with concatenation if there exists a
     * combination of addition, multiplication, and concatenation of the values
     * that results in the test value.
     *
     * @return `true` if this equation is solvable with concatenation, otherwise
     * `false`.
     */
    bool is_solvable_with_concat() const noexcept {
        return values.empty()
            ? (testValue == 0)
            : is_solvable_with_concat(1, values[0]);
    }

    /**
     * @brief Returns the test value of this equation.
     *
     * @return The test value of this equation.
     */
    i64 test_value() const noexcept {
        return testValue;
    }

};

/**
 * @brief Parses the equations from the standard input stream.
 *
 * The input is expected to consist of zero or more lines of text, where each
 * line represents an equation in the format as described in the documentation
 * of `Equation::parse()`. The individual lines must be separated by a newline
 * character.
 *
 * @return The parsed equations on success, or `std::nullopt` on failure.
 */
static inline std::optional<std::vector<Equation>> parse_equations() {
    std::vector<Equation> equations;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::optional<Equation> equation = Equation::parse(line);
        if (!equation) {
            return std::nullopt;
        }
        equations.push_back(std::move(*equation));
    }
    return equations;
}

int main() {
    std::optional<std::vector<Equation>> equations = parse_equations();
    if (!equations) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    i64 sum = std::accumulate(
        equations->begin(),
        equations->end(),
        (i64) 0,
        [](i64 acc, const Equation& e) {
            return acc + (e.is_solvable() ? e.test_value() : 0);
        }
    );
    i64 sumConcat = std::accumulate(
        equations->begin(),
        equations->end(),
        (i64) 0,
        [](i64 acc, const Equation& e) {
            return acc + (e.is_solvable_with_concat() ? e.test_value() : 0);
        }
    );
    std::println(
        "The sum of the test values of all solvable equations is {}.",
        sum
    );
    std::println(
        "The sum of the test values of all solvable equations with "
            "concatenation is {}.",
        sumConcat
    );
    return EXIT_SUCCESS;
}