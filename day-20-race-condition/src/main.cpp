#include <cassert>
#include <cmath>
#include <cstdlib>
#include <generator>
#include <iostream>
#include <optional>
#include <print>
#include <queue>
#include <unordered_map>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a two-dimensional position. */
struct Position {

    /** @brief The x-coordinate of this position. */
    isize x;

    /** @brief The y-coordinate of this position. */
    isize y;

    /**
     * @brief Determines whether two positions are equal.
     *
     * @param[in] lhs The first position.
     * @param[in] rhs The second position.
     * @return `true` if the positions are equal, otherwise `false`.
     */
    friend bool operator==(const Position& lhs, const Position& rhs) noexcept {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }

};

template<>
struct std::hash<Position> {
    usize operator()(const Position& pos) const noexcept {
        usize hash = 0;
        hash ^= std::hash<isize>()(pos.x) + 0x9E3779B9 + (hash << 6)
            + (hash >> 2);
        hash ^= std::hash<isize>()(pos.y) + 0x9E3779B9 + (hash << 6)
            + (hash >> 2);
        return hash;
    }
};

/** @brief Represents a map of the racetrack. */
class Map final {
private:

    /** @brief Represents a tile on the racetrack. */
    enum class Tile {
        Track,
        Wall
    };

    /** @brief The threshold (saved time) for the best cheats. */
    static constexpr isize BEST_CHEAT_THRESHOLD = 100;

    /** @brief The tiles of the racetrack. */
    std::vector<Tile> _tiles;

    /** @brief The width of the racetrack. */
    isize _width;

    /** @brief The height of the racetrack. */
    isize _height;

    /** @brief The start position of the racetrack. */
    Position _start;

    /** @brief The end position of the racetrack. */
    Position _end;

    /**
     * @brief Initializes a new map with the specified tiles, width, and height.
     *
     * @warning The behavior is undefined if `width` or `height` is negative, if
     * the number of tiles is not equal to `width * height`, or if `start` or
     * `end` is out of bounds.
     *
     * @param[in] tiles  The tiles of the racetrack.
     * @param[in] width  The width of the racetrack.
     * @param[in] height The height of the racetrack.
     * @param[in] start  The start position of the racetrack.
     * @param[in] end    The end position of the racetrack.
     */
    Map(
        std::vector<Tile> tiles,
        isize width,
        isize height,
        Position start,
        Position end
    ) noexcept
        : _tiles(std::move(tiles)),
          _width(width),
          _height(height),
          _start(start),
          _end(end) {
        assert(width >= 0);
        assert(height >= 0);
        assert(std::ssize(_tiles) == width * height);
        assert(exists(_start));
        assert(exists(_end));
    }

    /**
     * @brief Determines whether a specified position exists within the bounds
     * of the racetrack.
     *
     * @param[in] pos The position to check.
     * @return `true` if the position exists within the bounds of the racetrack,
     * otherwise `false`.
     */
    bool exists(const Position& pos) const noexcept {
        return (pos.x >= 0) && (pos.x < _width)
            && (pos.y >= 0) && (pos.y < _height);
    }

    /**
     * @brief Returns the accessible neighbors of a specified position on the
     * racetrack.
     *
     * @param[in] pos The position to get the neighbors of.
     * @return The accessible neighbors of the specified position on the
     * racetrack.
     */
    std::generator<Position> neighbors(const Position& pos) const {
        auto is_accessible = [this](const Position& p) -> bool {
            return exists(p) && (_tiles[p.y * _width + p.x] == Tile::Track);
        };
        Position left = { .x = pos.x - 1, .y = pos.y };
        if (is_accessible(left)) {
            co_yield left;
        }
        Position right = { .x = pos.x + 1, .y = pos.y };
        if (is_accessible(right)) {
            co_yield right;
        }
        Position up = { .x = pos.x, .y = pos.y - 1 };
        if (is_accessible(up)) {
            co_yield up;
        }
        Position down = { .x = pos.x, .y = pos.y + 1 };
        if (is_accessible(down)) {
            co_yield down;
        }
    }

    /**
     * @brief Returns a map of positions to the times it takes to reach them
     * from a specified source position on the racetrack.
     *
     * @param[in] src The source position.
     * @return A map of positions to their times from the source position.
     */
    std::unordered_map<Position, isize> times_from(const Position& src) const {
        assert(exists(src));
        std::unordered_map<Position, isize> times;
        times[src] = 0;
        std::queue<Position> queue;
        queue.push(src);
        while (!queue.empty()) {
            Position pos = queue.front();
            queue.pop();
            for (const Position& neighbor : neighbors(pos)) {
                if (!times.contains(neighbor)) {
                    times[neighbor] = times[pos] + 1;
                    queue.push(neighbor);
                }
            }
        }
        return times;
    }

public:

    /**
     * @brief Parses a map from the standard input stream.
     *
     * The input must consist of zero or more lines of the same length, each
     * containing only the characters `.` (for a track tile) and `#` (for a wall
     * tile). Additionally, the input must contain exactly one `S` character
     * (marking the start position) and exactly one `E` character (marking the
     * end position). The `S` and `E` characters are treated as track tiles.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * ###############
     * #...#...#.....#
     * #.#.#.#.#.###.#
     * #S#...#.#.#...#
     * #######.#.#.###
     * #######.#.#...#
     * #######.#.###.#
     * ###..E#...#...#
     * ###.#######.###
     * #...###...#...#
     * #.#####.#.###.#
     * #.#...#.#.#...#
     * #.#.#.#.#.#.###
     * #...#...#...###
     * ###############
     * ```
     *
     * @return The parsed map on success, or `std::nullopt` on failure.
     */
    static std::optional<Map> parse() {
        std::vector<Tile> tiles;
        isize width = 0;
        isize height = 0;
        Position start;
        Position end;
        bool foundWidth = false;
        bool foundStart = false;
        bool foundEnd = false;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!foundWidth) {
                width = std::ssize(line);
                foundWidth = true;
            }
            else if (std::ssize(line) != width) {
                return std::nullopt;
            }
            for (isize i = 0; i < width; i++) {
                switch (line[i]) {
                    case '.':
                        tiles.push_back(Tile::Track);
                        break;
                    case '#':
                        tiles.push_back(Tile::Wall);
                        break;
                    case 'S':
                        if (foundStart) {
                            return std::nullopt;
                        }
                        start = { .x = i, .y = height };
                        foundStart = true;
                        tiles.push_back(Tile::Track);
                        break;
                    case 'E':
                        if (foundEnd) {
                            return std::nullopt;
                        }
                        end = { .x = i, .y = height };
                        foundEnd = true;
                        tiles.push_back(Tile::Track);
                        break;
                    default:
                        return std::nullopt;
                }
            }
            height++;
        }
        if (!foundStart || !foundEnd) {
            return std::nullopt;
        }
        return Map(std::move(tiles), width, height, start, end);
    }

    /**
     * @brief Returns the number of best cheats that can be made to complete the
     * race in the shortest possible time.
     *
     * Programs can cheat once per race by ignoring collisions with walls for at
     * most `maxCheatTime` consecutive moves. A cheat can start and end at any
     * point in time, but it must start and end on a track tile.
     *
     * @param[in] maxCheatTime The maximum number of consecutive moves that can
     *                         be made while cheating.
     * @return The number of best cheats that can be made to complete the race
     * in the shortest possible time.
     */
    isize best_cheats(isize maxCheatTime) const {
        std::unordered_map<Position, isize> timesFromStart = times_from(_start);
        std::unordered_map<Position, isize> timesFromEnd = times_from(_end);
        isize fastestTime = timesFromStart[_end];
        isize bestCheats = 0;
        for (const auto& [src, time] : timesFromStart) {
            for (isize dy = -maxCheatTime; dy <= maxCheatTime; dy++) {
                for (isize dx = -maxCheatTime; dx <= maxCheatTime; dx++) {
                    isize cheatTime = std::abs(dx) + std::abs(dy);
                    if ((cheatTime == 0) || (cheatTime > maxCheatTime)) {
                        continue;
                    }
                    Position dst = { .x = src.x + dx, .y = src.y + dy };
                    auto it = timesFromEnd.find(dst);
                    if (it == timesFromEnd.end()) {
                        continue;
                    }
                    isize totalTime = time + cheatTime + it->second;
                    if (fastestTime - totalTime >= BEST_CHEAT_THRESHOLD) {
                        bestCheats++;
                    }
                }
            }
        }
        return bestCheats;
    }

};

int main() {
    std::optional<Map> map = Map::parse();
    if (!map) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize bestCheats2 = map->best_cheats(2);
    isize bestCheats20 = map->best_cheats(20);
    std::println(
        "The best number of cheats with at most 2 moves is {}.",
        bestCheats2
    );
    std::println(
        "The best number of cheats with at most 20 moves is {}.",
        bestCheats20
    );
    return EXIT_SUCCESS;
}