#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <vector>

/** @brief Represents the notes of the historian. */
class Notes final {
private:

    /** @brief The left list of numbers. */
    std::vector<std::int64_t> left;

    /** @brief The right list of numbers. */
    std::vector<std::int64_t> right;

    /** @brief The frequencies of the numbers in the right list. */
    std::unordered_map<std::int64_t, std::int64_t> rightFrequencies;

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
    Notes(std::vector<std::int64_t> left, std::vector<std::int64_t> right)
        : left(std::move(left)), right(std::move(right)) {
        assert(this->left.size() == this->right.size());
        std::ranges::sort(this->left);
        std::ranges::sort(this->right);
        for (std::int64_t r : this->right) {
            rightFrequencies[r]++;
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
        std::vector<std::int64_t> left;
        std::vector<std::int64_t> right;
        std::string line;
        while (std::getline(std::cin, line)) {
            std::istringstream iss(line);
            std::int64_t l;
            std::int64_t r;
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
    std::int64_t total_distance() const {
        std::int64_t distance = 0;
        for (const auto [l, r] : std::views::zip(left, right)) {
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
    std::int64_t similarity_score() const {
        std::int64_t score = 0;
        for (std::int64_t l : left) {
            auto frequency = rightFrequencies.find(l);
            if (frequency != rightFrequencies.end()) {
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