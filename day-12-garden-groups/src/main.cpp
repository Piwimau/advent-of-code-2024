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

/** @brief Represents a two-dimensional map of garden plots. */
struct Map {
private:

    /** @brief Represents a two-dimensional position. */
    struct Position {

        /** @brief The x-coordinate of the position. */
        isize x;

        /** @brief The y-coordinate of the position. */
        isize y;

    };

    /** @brief The garden plots of the map. */
    std::vector<char> plots;

    /** @brief The width of the map. */
    isize width;

    /** @brief The height of the map. */
    isize height;

    /**
     * @brief Initializes a new map with the specified garden plots, width, and
     * height.
     *
     * @warning The behavior is undefined if the number of garden plots is not
     * equal to `width * height`, or if `width` or `height` is negative.
     *
     * @param[in] plots The garden plots of the map.
     * @param[in] width The width of the map.
     * @param[in] height The height of the map.
     */
    Map(std::vector<char> plots, isize width, isize height)
        : plots(std::move(plots)),
          width(width),
          height(height) {
        assert(width >= 0);
        assert(height >= 0);
        assert(std::ssize(this->plots) == (width * height));
    }

    /**
     * @brief Returns all neighbors of a specified position.
     *
     * @param[in] pos The position whose neighbors to return.
     * @return A generator that yields all neighbors of the specified position.
     */
    std::generator<Position> neighbors(Position pos) const noexcept {
        co_yield { pos.x - 1, pos.y };
        co_yield { pos.x + 1, pos.y };
        co_yield { pos.x, pos.y - 1 };
        co_yield { pos.x, pos.y + 1 };
    }

    /**
     * @brief Determines whether a specified position exists on this map.
     *
     * @param[in] pos The position to check.
     * @return `true` if the specified position exists on this map, otherwise
     * `false`.
     */
    bool exists(Position pos) const noexcept {
        return (pos.x >= 0) && (pos.x < width)
            && (pos.y >= 0) && (pos.y < height);
    }

    /**
     * @brief Tries to get the plant at a specified position.
     *
     * @param[in] pos The position to get the plant at.
     * @return The plant at the specified position on success, or `std::nullopt`
     * if the specified position does not exist on this map.
     */
    std::optional<char> plant_at(Position pos) const noexcept {
        return exists(pos)
            ? std::make_optional(plots[(pos.y * width) + pos.x])
            : std::nullopt;
    }

    /**
     * @brief Counts the number of sides of a region a specified position is
     * adjacent to.
     *
     * @param[in] pos The position to count the sides for.
     * @return The number of sides of a region the specified position is
     * adjacent to.
     */
    isize count_sides(Position pos) const noexcept {
        std::optional<char> center = plant_at(pos);
        assert(center);
        std::optional<char> w = plant_at({ pos.x - 1, pos.y });
        std::optional<char> nw = plant_at({ pos.x - 1, pos.y - 1 });
        std::optional<char> n = plant_at({ pos.x, pos.y - 1 });
        std::optional<char> ne = plant_at({ pos.x + 1, pos.y - 1 });
        std::optional<char> e = plant_at({ pos.x + 1, pos.y });
        std::optional<char> se = plant_at({ pos.x + 1, pos.y + 1 });
        std::optional<char> s = plant_at({ pos.x, pos.y + 1 });
        std::optional<char> sw = plant_at({ pos.x - 1, pos.y + 1 });
        isize sides = 0;
        if ((w != center) && (n != center)) {
            sides++;
        }
        if ((w == center) && (n == center) && (nw != center)) {
            sides++;
        }
        if ((n != center) && (e != center)) {
            sides++;
        }
        if ((n == center) && (e == center) && (ne != center)) {
            sides++;
        }
        if ((e != center) && (s != center)) {
            sides++;
        }
        if ((e == center) && (s == center) && (se != center)) {
            sides++;
        }
        if ((s != center) && (w != center)) {
            sides++;
        }
        if ((s == center) && (w == center) && (sw != center)) {
            sides++;
        }
        return sides;
    }

public:

    /**
     * @brief Parses a map from the standard input stream.
     *
     * The input is expected to consist of zero or more lines of text, each
     * containing the same number of characters. Each character represents a
     * garden plot (and the type of plant growing in that plot). The individual
     * lines must be separated with a newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * AAAA
     * BBCD
     * BBCC
     * EEEC
     * ```
     *
     * @return The parsed map on success, or `std::nullopt` on failure.
     */
    static std::optional<Map> parse() {
        std::vector<char> plots;
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
            plots.insert_range(plots.end(), line);
            height++;
        }
        return Map(std::move(plots), width, height);
    }

    /**
     * @brief Computes the price of this map.
     *
     * The price of a map is the sum of the prices of all regions on the map,
     * where the price of a region is the product of its area and its perimeter.
     *
     * @return The price of this map.
     */
    isize price() const {
        isize price = 0;
        auto visited = std::make_unique<bool[]>(width * height);
        std::queue<Position> queue;
        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {
                isize idx = (y * width) + x;
                if (visited[idx]) {
                    continue;
                }
                visited[idx] = true;
                isize area = 0;
                isize perimeter = 0;
                queue.emplace(x, y);
                while (!queue.empty()) {
                    Position pos = queue.front();
                    queue.pop();
                    area++;
                    for (Position neighbor : neighbors(pos)) {
                        std::optional<char> neighborPlant = plant_at(neighbor);
                        if (neighborPlant != plots[idx]) {
                            perimeter++;
                            continue;
                        }
                        isize neighborIdx = (neighbor.y * width) + neighbor.x;
                        if (!visited[neighborIdx]) {
                            visited[neighborIdx] = true;
                            queue.push(neighbor);
                        }
                    }
                }
                price += (area * perimeter);
            }
        }
        return price;
    }

    /**
     * @brief Computes the price of this map under the bulk discount.
     *
     * Under the bulk discount, the price of a map is the sum of the prices of
     * all regions on the map, where the price of a region is the product of its
     * area and the number of sides it has.
     *
     * @return The price of this map under the bulk discount.
     */
    isize price_with_bulk_discount() const {
        isize price = 0;
        auto visited = std::make_unique<bool[]>(width * height);
        std::queue<Position> queue;
        for (isize y = 0; y < height; y++) {
            for (isize x = 0; x < width; x++) {
                isize idx = (y * width) + x;
                if (visited[idx]) {
                    continue;
                }
                visited[idx] = true;
                isize area = 0;
                isize sides = 0;
                queue.emplace(x, y);
                while (!queue.empty()) {
                    Position pos = queue.front();
                    queue.pop();
                    area++;
                    sides += count_sides(pos);
                    for (Position neighbor : neighbors(pos)) {
                        isize neighborIdx = (neighbor.y * width) + neighbor.x;
                        if (
                            !visited[neighborIdx]
                                && (plant_at(neighbor) == plots[idx])
                        ) {
                            visited[neighborIdx] = true;
                            queue.push(neighbor);
                        }
                    }
                }
                price += (area * sides);
            }
        }
        return price;
    }

};

int main() {
    std::optional<Map> map = Map::parse();
    if (!map) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize price = map->price();
    isize priceBulk = map->price_with_bulk_discount();
    std::println("The price of all regions on the map is {}.", price);
    std::println(
        "The price of all regions on the map with bulk discount is {}.",
        priceBulk
    );
    return EXIT_SUCCESS;
}