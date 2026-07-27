#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <print>
#include <string>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/**
 * @brief Parses the initial seeds from the standard input stream.
 *
 * The input must consist of zero or more lines, each containing a single seed.
 * Each seed itself must be a non-negative integer.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * 1
 * 10
 * 100
 * 2024
 * ```
 *
 * @return The parsed seeds on success, or `std::nullopt` on failure.
 */
static std::optional<std::vector<usize>> parse_seeds() {
    std::vector<usize> seeds;
    std::string line;
    while (std::getline(std::cin, line)) {
        auto [_, ec] = std::from_chars(
            line.data(),
            line.data() + std::ssize(line),
            seeds.emplace_back()
        );
        if (ec != std::errc()) {
            return std::nullopt;
        }
    }
    return seeds;
}

/**
 * @brief Returns the next secret number in the sequence for a specified seed.
 *
 * @param[in] seed The initial seed.
 * @return The next secret number in the sequence for the specified seed.
 */
static inline usize next(usize seed) noexcept {
    seed = (seed ^ (seed << 6)) & 0xFFFFFF;
    seed = (seed ^ (seed >> 5)) & 0xFFFFFF;
    seed = (seed ^ (seed << 11)) & 0xFFFFFF;
    return seed;
}

/**
 * @brief Calculates the sum of the 2000th secret numbers for a specified list
 * of initial seeds.
 *
 * @param[in] seeds The list of initial seeds.
 * @return The sum of the 2000th secret numbers for the specified list of
 * initial seeds.
 */
static inline usize sum_of_2000th_secret_numbers(
    const std::vector<usize>& seeds
) noexcept {
    return std::transform_reduce(
        seeds.begin(),
        seeds.end(),
        static_cast<usize>(0),
        std::plus<usize>(),
        [](usize seed) -> usize {
            for (usize i = 0; i < 2000; i++) {
                seed = next(seed);
            }
            return seed;
        }
    );
}

/**
 * @brief Calculates the maximum number of bananas that can be acquired using
 * a specified list of initial seeds.
 *
 * @param[in] seeds The list of initial seeds.
 * @return The maximum number of bananas that can be acquired using the
 * specified list of initial seeds.
 */
static usize max_bananas(const std::vector<usize>& seeds) noexcept {
    // We represent the sequence of four price differences as a 20-bit integer,
    // where each 5-bit block corresponds to one of the 19 possible price
    // differences (i.e., -9 to 9).
    constexpr usize MaxSequences = 1 << 20;
    std::unique_ptr<usize[]> bananas = std::make_unique<usize[]>(MaxSequences);
    std::array<usize, 2001> prices;
    std::unique_ptr<bool[]> seen = std::make_unique<bool[]>(MaxSequences);
    for (usize seed : seeds) {
        for (usize& price : prices) {
            price = seed % 10;
            seed = next(seed);
        }
        std::ranges::fill_n(seen.get(), MaxSequences, false);
        for (usize i = 0; i < std::ssize(prices) - 4; i++) {
            usize p0 = prices[i];
            usize p1 = prices[i + 1];
            usize p2 = prices[i + 2];
            usize p3 = prices[i + 3];
            usize p4 = prices[i + 4];
            usize d0 = (p1 - p0) & 0x1F;
            usize d1 = (p2 - p1) & 0x1F;
            usize d2 = (p3 - p2) & 0x1F;
            usize d3 = (p4 - p3) & 0x1F;
            usize sequence = (d0 << 15) | (d1 << 10) | (d2 << 5) | d3;
            if (!seen[sequence]) {
                seen[sequence] = true;
                bananas[sequence] += p4;
            }
        }
    }
    return *std::ranges::max_element(
        bananas.get(),
        bananas.get() + MaxSequences
    );
}

int main() {
    std::optional<std::vector<usize>> seeds = parse_seeds();
    if (!seeds) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    usize sum = sum_of_2000th_secret_numbers(*seeds);
    usize max = max_bananas(*seeds);
    std::println("The sum of the 2000th secret numbers is {}.", sum);
    std::println(
        "The maximum number of bananas that can be acquired is {}.",
        max
    );
    return EXIT_SUCCESS;
}