#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents the notes of the historian. */
class Notes final {
private:

    /** @brief The left list of numbers. */
    std::vector<i32> _left;

    /** @brief The right list of numbers. */
    std::vector<i32> _right;

    /** @brief The frequencies of the numbers in the right list. */
    std::unordered_map<i32, i32> _rightFrequencies;

    /**
     * @brief Initializes a new `Notes` instance with the specified lists of
     * numbers.
     *
     * @warning The behavior is undefined if `left` and `right` do not have the
     * same number of elements.
     *
     * @param[in] left  The left list of numbers.
     * @param[in] right The right list of numbers.
     */
    Notes(std::vector<i32> left, std::vector<i32> right)
        : _left(std::move(left)), _right(std::move(right)) {
        assert(_left.size() == _right.size());
        std::ranges::sort(_left);
        std::ranges::sort(_right);
        for (i32 r : _right) {
            _rightFrequencies[r]++;
        }
    }

public:

    /**
     * @brief Parses notes from the standard input stream.
     *
     * The input must consist of zero or more lines with the following format:
     *
     * ```plaintext
     * <left> <right>
     * ```
     *
     * Here, `<left>` and `<right>` are two integers separated by one or more
     * spaces. The individual lines must be separated by a newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * 3   4
     * 4   3
     * 2   5
     * 1   3
     * 3   9
     * 3   3
     * ```
     *
     * @return The parsed notes on success, or `std::nullopt` on failure.
     */
    static std::optional<Notes> parse() {
        std::vector<i32> left;
        std::vector<i32> right;
        std::string line;
        while (std::getline(std::cin, line)) {
            std::istringstream iss(line);
            i32 l;
            i32 r;
            if (!(iss >> l >> r)) {
                return std::nullopt;
            }
            left.push_back(l);
            right.push_back(r);
        }
        return Notes(std::move(left), std::move(right));
    }

    /**
     * @brief Returns the total distance.
     *
     * The total distance is defined as the sum of the absolute differences
     * between the numbers in the left and right lists.
     *
     * @return The total distance.
     */
    i32 total_distance() const {
        i32 distance = 0;
        for (const auto [l, r] : std::views::zip(_left, _right)) {
            distance += std::abs(l - r);
        }
        return distance;
    }

    /**
     * @brief Returns the similarity score.
     *
     * The similarity score is defined as the sum of the products of the numbers
     * in the left and right sets, where each number in the left set is
     * multiplied with the number of times it appears in the right set.
     *
     * @return The similarity score.
     */
    i32 similarity_score() const {
        i32 score = 0;
        for (i32 l : _left) {
            auto frequency = _rightFrequencies.find(l);
            if (frequency != _rightFrequencies.end()) {
                score += l * frequency->second;
            }
        }
        return score;
    }

};

int main() {
    std::optional<Notes> notes = Notes::parse();
    if (!notes) {
        std::println(
            stderr,
            "An error occurred while reading the input file."
        );
        return EXIT_FAILURE;
    }
    std::println("The total distance is {}.", notes->total_distance());
    std::println("The similarity score is {}.", notes->similarity_score());
    return EXIT_SUCCESS;
}