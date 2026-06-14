#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <generator>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <queue>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a topographic map. */
class Map final {
private:

    /** @brief Represents a two-dimensional position. */
    struct Position {

        /** @brief The x-coordinate of the position. */
        isize x;

        /** @brief The y-coordinate of the position. */
        isize y;

    };

    /** @brief The maximum height of any position on the map. */
    static constexpr i32 MAX_HEIGHT = 9;

    /** @brief The heights of the positions on the map. */
    std::vector<i32> heights;

    /** @brief The width of the map. */
    isize width;

    /** @brief The height of the map. */
    isize height;

    /**
     * @brief Initializes a new map with the given heights, width, and height.
     *
     * @warning The behavior is undefined if `heights` does not have `width *
     * height` elements, or if `width` or `height` is negative.
     *
     * @param[in] heights The heights of the positions on the map.
     * @param[in] width   The width of the map.
     * @param[in] height  The height of the map.
     */
    Map(std::vector<i32> heights, isize width, isize height)
        : heights(std::move(heights)),
          width(width),
          height(height) {
        assert(std::ssize(this->heights) == (width * height));
        assert(width >= 0);
        assert(height >= 0);
    }

    /**
     * @brief Returns the existing neighbors of a specified position.
     *
     * @param[in] pos The position whose neighbors to return.
     * @return The existing neighbors of the specified position.
     */
    std::generator<Position> neighbors(Position pos) const noexcept {
        if (pos.x > 0) {
            co_yield { pos.x - 1, pos.y };
        }
        if (pos.x < (width - 1)) {
            co_yield { pos.x + 1, pos.y };
        }
        if (pos.y > 0) {
            co_yield { pos.x, pos.y - 1 };
        }
        if (pos.y < (height - 1)) {
            co_yield { pos.x, pos.y + 1 };
        }
    }

public:

    /**
     * @brief Parses a topographic map from the standard input stream.
     *
     * The input must consist of zero or more lines of digits, where each digit
     * is between `0` and `9` inclusive and represents the height of a position
     * on the map. The map must be rectangular, i.e., all lines must have the
     * same number of digits. The individual lines must be separated with a
     * newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * 0123
     * 1234
     * 8765
     * 9876
     * ```
     *
     * @return The parsed map on success, or `std::nullopt` on failure.
     */
    static std::optional<Map> parse() {
        std::vector<i32> heights;
        isize width = 0;
        isize height = 0;
        bool foundWidth = false;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!foundWidth) {
                width = std::ssize(line);
                foundWidth = true;
            }
            else if (std::ssize(line) != width) {
                return std::nullopt;
            }
            for (char c : line) {
                if ((c < '0') || (c > '9')) {
                    return std::nullopt;
                }
                heights.push_back(c - '0');
            }
            height++;
        }
        return Map(std::move(heights), width, height);
    }

    /**
     * @brief Returns the sum of the scores of all trailheads on this map.
     *
     * A trailhead is any position that starts one or more hiking trails, which
     * are paths that start at a position of height `0` and only extend to
     * adjacent positions of a height that is exactly one greater than the
     * current position. A trailhead's score is the number of `9`-height
     * positions reachable from that trailhead via a hiking trail.
     *
     * @return The sum of the scores of all trailheads on this map.
     */
    isize sum_of_trailhead_scores() const {
        isize sum = 0;
        auto visited = std::make_unique<bool[]>(width * height);
        std::deque<Position> queue;
        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {
                if (heights[y * width + x] != 0) {
                    continue;
                }
                isize score = 0;
                std::ranges::fill_n(visited.get(), width * height, false);
                visited[y * width + x] = true;
                queue.clear();
                queue.emplace_back(x, y);
                while (!queue.empty()) {
                    Position pos = queue.front();
                    queue.pop_front();
                    if (heights[pos.y * width + pos.x] == MAX_HEIGHT) {
                        score++;
                        continue;
                    }
                    for (Position neighbor : neighbors(pos)) {
                        if (visited[neighbor.y * width + neighbor.x]) {
                            continue;
                        }
                        if (
                            heights[neighbor.y * width + neighbor.x]
                                != (heights[pos.y * width + pos.x] + 1)
                        ) {
                            continue;
                        }
                        visited[neighbor.y * width + neighbor.x] = true;
                        queue.push_back(neighbor);
                    }
                }
                sum += score;
            }
        }
        return sum;
    }

    /**
     * @brief Returns the sum of the ratings of all trailheads on this map.
     *
     * A trailhead is any position that starts one or more hiking trails, which
     * are paths that start at a position of height `0` and only extend to
     * adjacent positions of a height that is exactly one greater than the
     * current position. A trailhead's rating is the number of distinct hiking
     * trails that start at that trailhead.
     *
     * @return The sum of the ratings of all trailheads on this map.
     */
    isize sum_of_trailhead_ratings() const {
        isize sum = 0;
        std::deque<Position> queue;
        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {
                if (heights[y * width + x] != 0) {
                    continue;
                }
                isize score = 0;
                queue.clear();
                queue.emplace_back(x, y);
                while (!queue.empty()) {
                    Position pos = queue.front();
                    queue.pop_front();
                    if (heights[pos.y * width + pos.x] == MAX_HEIGHT) {
                        score++;
                        continue;
                    }
                    for (Position neighbor : neighbors(pos)) {
                        if (
                            heights[neighbor.y * width + neighbor.x]
                                != (heights[pos.y * width + pos.x] + 1)
                        ) {
                            continue;
                        }
                        queue.push_back(neighbor);
                    }
                }
                sum += score;
            }
        }
        return sum;
    }

};

int main() {
    std::optional<Map> map = Map::parse();
    if (!map) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize sumOfScores = map->sum_of_trailhead_scores();
    isize sumOfRatings = map->sum_of_trailhead_ratings();
    std::println("The sum of the scores of all trailheads is {}.", sumOfScores);
    std::println(
        "The sum of the ratings of all trailheads is {}.",
        sumOfRatings
    );
    return EXIT_SUCCESS;
}