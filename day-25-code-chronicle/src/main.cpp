#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a five-pin tumbler lock. */
class Lock final {
public:

    /** @brief The number of pins in a lock. */
    static constexpr isize PINS = 5;

    /** @brief The maximum height of a single pin. */
    static constexpr isize MAX_PIN_HEIGHT = 6;

private:

    /** @brief The heights of the pins of this lock. */
    std::array<isize, PINS> _heights;

public:

    /**
     * @brief Initializes a new lock with the specified pin heights.
     *
     * @param[in] heights The heights of the pins of the lock.
     */
    constexpr explicit Lock(std::array<isize, PINS> heights) noexcept
        : _heights(std::move(heights)) { }

    /**
     * @brief Returns the heights of the pins of this lock.
     *
     * @return The heights of the pins of this lock.
     */
    constexpr const std::array<isize, PINS>& heights() const noexcept {
        return _heights;
    }

};

/** @brief Represents a key for a five-pin tumbler lock. */
class Key final {
private:

    /** @brief The heights of the teeth of this key. */
    std::array<isize, Lock::PINS> _heights;

public:

    /**
     * @brief Initializes a new key with the specified teeth heights.
     *
     * @param[in] heights The heights of the teeth of the key.
     */
    constexpr explicit Key(std::array<isize, Lock::PINS> heights) noexcept
        : _heights(std::move(heights)) { }

    /**
     * @brief Determines whether this key fits into a lock.
     *
     * @param[in] lock The lock to try.
     * @return `true ` if this key fits into the lock, otherwise `false`.
     */
    constexpr bool fits_into(const Lock& lock) const noexcept {
        return std::ranges::all_of(
            std::views::zip_transform(
                std::plus<isize>(),
                lock.heights(),
                _heights
            ),
            [](isize sum) { return sum <= Lock::MAX_PIN_HEIGHT; }
        );
    }

};

/**
 * @brief Parses the locks and keys from the standard input stream.
 *
 * The input must consist of zero or more lines of text that describe the shape
 * of locks and keys. Each lock or key is represented as one or more lines of
 * text and separated from the next lock or key using an empty line. The
 * characters within such a line must be either `.`  or `#`. For a block of
 * lines to be identified as a lock, the first line must be filled with `#`. The
 * opposite holds for keys (i.e., the last line must be filled with `#`).
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * #####
 * .####
 * .####
 * .####
 * .#.#.
 * .#...
 * .....
 *
 * .....
 * #....
 * #....
 * #...#
 * #.#.#
 * #.###
 * #####
 * ```
 *
 * @return The parsed locks and keys on success, or `std::nullopt` on failure.
 */
static std::optional<std::pair<std::vector<Lock>, std::vector<Key>>>
parse_locks_and_keys() {
    std::vector<Lock> locks;
    std::vector<Key> keys;
    bool isLock = false;
    bool isFirstLine = true;
    std::array<isize, Lock::PINS> heights = { };
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            if (isLock) {
                locks.emplace_back(heights);
            }
            else {
                keys.emplace_back(heights);
            }
            isFirstLine = true;
            continue;
        }
        if (
            (std::ssize(line) != Lock::PINS)
                || std::ranges::any_of(
                    line,
                    [](char c) { return (c != '.') && (c != '#'); }
                )
        ) {
            return std::nullopt;
        }
        if (isFirstLine) {
            isLock = std::ranges::all_of(line, [](char c) { return c == '#'; });
            std::ranges::fill(heights, 0);
            isFirstLine = false;
            continue;
        }
        for (isize i = 0; i < Lock::PINS; i++) {
            heights[i] += (line[i] == '#') ? 1 : 0;
            if (heights[i] > Lock::MAX_PIN_HEIGHT) {
                return std::nullopt;
            }
        }
    }
    // We have to manually add the last lock or key, since the input might not
    // end with an empty line.
    if (!isFirstLine) {
        if (isLock) {
            locks.emplace_back(heights);
        }
        else {
            keys.emplace_back(heights);
        }
    }
    return std::make_pair(locks, keys);
}

int main() {
    auto locksAndKeys = parse_locks_and_keys();
    if (!locksAndKeys) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    auto& [locks, keys] = *locksAndKeys;
    isize fitting = std::ranges::count_if(
        std::views::cartesian_product(keys, locks),
        [](const auto& pair) {
            const auto& [key, lock] = pair;
            return key.fits_into(lock);
        }
    );
    std::println("There are {} lock/key pairs that fit together.", fitting);
    return EXIT_SUCCESS;
}