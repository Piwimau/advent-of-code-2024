#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/**
 * @brief Parses a list of stones from the standard input stream.
 *
 * The input is expected to consist of a single line containing zero or more
 * positive integers separated by spaces. Each integer represents the number
 * engraved on a stone.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * 0 1 10 99 999
 * ```
 *
 * @return The parsed list of stones on success, or `std::nullopt` on failure.
 */
static std::optional<std::vector<isize>> parse_stones() {
    std::vector<isize> stones;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return std::nullopt;
    }
    std::istringstream iss(line);
    isize stone;
    while (iss >> stone) {
        stones.push_back(stone);
    }
    return stones;
}

/**
 * @brief Computes the number of stones after blinking a specified number of
 * times.
 *
 * @param[in] stones The stones for the simulation.
 * @param[in] times  The number of times to blink.
 * @return The number of stones after blinking the specified number of times.
 */
static isize blink(const std::vector<isize>& stones, isize times) {
    assert(times >= 0);
    std::unordered_map<isize, isize> oldFreqs;
    std::unordered_map<isize, isize> newFreqs;
    for (isize stone : stones) {
        oldFreqs[stone]++;
    }
    for (isize i = 0; i < times; i++) {
        for (const auto& [stone, freq] : oldFreqs) {
            if (stone == 0) {
                newFreqs[1] += freq;
                continue;
            }
            isize digits = static_cast<isize>(std::floor(std::log10(stone)))
                + 1;
            if ((digits % 2) == 0) {
                isize mask = static_cast<isize>(std::pow(10, digits / 2));
                isize left = stone / mask;
                isize right = stone % mask;
                newFreqs[left] += freq;
                newFreqs[right] += freq;
                continue;
            }
            newFreqs[stone * 2024] += freq;
        }
        std::swap(oldFreqs, newFreqs);
        newFreqs.clear();
    }
    isize count = 0;
    for (const auto& [_, freq] : oldFreqs) {
        count += freq;
    }
    return count;
}

int main() {
    std::optional<std::vector<isize>> stones = parse_stones();
    if (!stones) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize num25 = blink(*stones, 25);
    isize num75 = blink(*stones, 75);
    std::println("After blinking 25 times, there are {} stones.", num25);
    std::println("After blinking 75 times, there are {} stones.", num75);
    return EXIT_SUCCESS;
}