#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a pattern of stripes. */
class Pattern final {
private:

    /** @brief Represents a stripe color of a towel. */
    enum class Stripe {
        White,
        Blue,
        Black,
        Red,
        Green
    };

    /** @brief The stripes that make up this pattern. */
    std::vector<Stripe> _stripes;

    /**
     * @brief Initializes a new pattern with the specified stripes.
     *
     * @warning The behavior is undefined if `stripes` is empty.
     *
     * @param[in] stripes The stripes that make up the pattern.
     */
    explicit Pattern(std::vector<Stripe> stripes) noexcept
        : _stripes(std::move(stripes)) {
        assert(!_stripes.empty());
    }

public:

    /**
     * @brief Parses a pattern from a specified string.
     *
     * @param[in] s The string to parse.
     * @return The parsed pattern on success, or `std::nullopt` on failure.
     */
    static std::optional<Pattern> parse(const std::string_view& s) {
        std::vector<Stripe> stripes;
        for (char c : s) {
            Stripe stripe;
            switch (c) {
                case 'w':
                    stripe = Stripe::White;
                    break;
                case 'u':
                    stripe = Stripe::Blue;
                    break;
                case 'b':
                    stripe = Stripe::Black;
                    break;
                case 'r':
                    stripe = Stripe::Red;
                    break;
                case 'g':
                    stripe = Stripe::Green;
                    break;
                default:
                    return std::nullopt;
            }
            stripes.push_back(stripe);
        }
        if (stripes.empty()) {
            return std::nullopt;
        }
        return Pattern(std::move(stripes));
    }

    /**
     * @brief Returns the number of ways this pattern can be built with a
     * specified set of patterns.
     *
     * @param[in] patterns The available patterns.
     * @return The number of ways this pattern can be built with the specified
     * set of patterns.
     */
    isize ways_of_building_with(std::span<const Pattern> patterns) const {
        isize n = std::ssize(_stripes);
        std::unique_ptr<isize[]> ways = std::make_unique<isize[]>(n + 1);
        ways[0] = 1;
        for (isize i = 0; i < n; i++) {
            if (ways[i] == 0) {
                continue;
            }
            for (const Pattern& pattern : patterns) {
                isize m = std::ssize(pattern._stripes);
                if (
                    (i + m <= n)
                        && std::ranges::equal(
                            std::span(_stripes).subspan(i, m),
                            pattern._stripes
                        )
                ) {
                    ways[i + m] += ways[i];
                }
            }
        }
        return ways[n];
    }

};

using Input = std::pair<std::vector<Pattern>, std::vector<Pattern>>;

/**
 * @brief Parses the input from the standard input stream.
 *
 * The input must consist of three or more lines with the following format:
 *
 * ```plaintext
 * <patterns>
 *
 * <designs>
 * ```
 *
 * Here, `<patterns>` is a list of towel patterns, each represented by a string
 * of characters (one of `w`, `u`, `b`, `r`, or `g`) that correspond to the
 * colors white, blue, black, red, and green, respectively. The patterns must be
 * on the same line, separated by a comma and a space each. The `<designs>`
 * section is a list of desired towel designs, each represented by a string of
 * characters in the same format as the patterns. In contrast to the patterns,
 * each design must be on a separate line. The `<patterns>` and `<designs>`
 * sections must be separated by a blank line.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * r, wr, b, g, bwu, rb, gb, br
 *
 * brwrr
 * bggr
 * gbbr
 * rrbgbr
 * ubwu
 * bwurrg
 * brgr
 * bbrgwb
 * ```
 *
 * @return The parsed input on success, or `std::nullopt` on failure.
 */
static std::optional<Input> parse_input() {
    std::vector<Pattern> patterns;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return std::nullopt;
    }
    usize idx = 0;
    usize separatorIdx = line.find(", ");
    while (separatorIdx != std::string::npos) {
        std::string_view s = std::string_view(line).substr(
            idx,
            separatorIdx - idx
        );
        std::optional<Pattern> pattern = Pattern::parse(s);
        if (!pattern) {
            return std::nullopt;
        }
        patterns.push_back(*pattern);
        idx = separatorIdx + 2;
        separatorIdx = line.find(", ", idx);
    }
    // The last pattern is not followed by a separator, so we need to handle it
    // separately.
    std::string_view s = std::string_view(line).substr(idx);
    std::optional<Pattern> pattern = Pattern::parse(s);
    if (!pattern) {
        return std::nullopt;
    }
    patterns.push_back(*pattern);
    if (!std::getline(std::cin, line) || !line.empty()) {
        return std::nullopt;
    }
    std::vector<Pattern> designs;
    while (std::getline(std::cin, line)) {
        std::optional<Pattern> design = Pattern::parse(line);
        if (!design) {
            return std::nullopt;
        }
        designs.push_back(*design);
    }
    if (designs.empty()) {
        return std::nullopt;
    }
    return std::make_pair(std::move(patterns), std::move(designs));
}

int main() {
    std::optional<Input> input = parse_input();
    if (!input) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    auto& [patterns, designs] = *input;
    isize possible = 0;
    isize ways = 0;
    for (const Pattern& design : designs) {
        isize w = design.ways_of_building_with(patterns);
        if (w > 0) {
            possible++;
            ways += w;
        }
    }
    std::println("The number of possible designs is {}.", possible);
    std::println("The number of ways to build the designs is {}.", ways);
    return EXIT_SUCCESS;
}