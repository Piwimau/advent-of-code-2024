#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <generator>
#include <iostream>
#include <limits>
#include <optional>
#include <print>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a tile on a map. */
enum class Tile {
    Empty,
    Wall
};

/** @brief Represents a direction for turning and moving. */
enum class Direction {
    West,
    North,
    East,
    South
};

/** @brief Represents a two-dimensional vector. */
struct Vector {

    /** @brief The x-component of this vector. */
    isize x;

    /** @brief The y-component of this vector. */
    isize y;

    /**
     * @brief Determines whether two vectors are equal.
     *
     * @param[in] lhs The first vector.
     * @param[in] rhs The second vector.
     * @return `true` if the two vectors are equal, otherwise `false`.
     */
    friend bool operator==(const Vector& lhs, const Vector& rhs) noexcept {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }

};

template<>
struct std::hash<Vector> {
    usize operator()(const Vector& vector) const noexcept {
        usize seed = std::hash<isize>()(vector.x);
        seed ^= std::hash<isize>()(vector.y) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        return seed;
    }
};

/** @brief Represents a state while traversing a map. */
struct State {

    /** @brief The current position on the map. */
    Vector pos;

    /** @brief The current direction we are facing. */
    Direction dir;

    /**
     * @brief Determines whether two states are equal.
     *
     * @param[in] lhs The first state.
     * @param[in] rhs The second state.
     * @return `true` if the two states are equal, otherwise `false`.
     */
    friend bool operator==(const State& lhs, const State& rhs) noexcept {
        return (lhs.pos == rhs.pos) && (lhs.dir == rhs.dir);
    }

};

template<>
struct std::hash<State> {
    usize operator()(const State& state) const noexcept {
        usize seed = std::hash<Vector>()(state.pos);
        seed ^= std::hash<Direction>()(state.dir) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        return seed;
    }
};

/** @brief Represents a map of the reindeer maze. */
class Map final {
private:

    /** @brief The tiles of this map. */
    std::vector<Tile> _tiles;

    /** @brief The width of this map. */
    isize _width;

    /** @brief The height of this map. */
    isize _height;

    /** @brief The starting position of the reindeer. */
    Vector _start;

    /** @brief The ending position of the reindeer. */
    Vector _end;

    /**
     * @brief Initializes a new map with the specified parameters.
     *
     * @warning The behavior is undefined if `tiles` does not have `width *
     * height` elements, if `width` or `height` is negative, or if `start` or
     * `end` is outside the bounds of the map.
     *
     * @param[in] tiles  The tiles of the map.
     * @param[in] width  The width of the map.
     * @param[in] height The height of the map.
     * @param[in] start  The starting position of the reindeer.
     * @param[in] end    The ending position of the reindeer.
     */
    Map(
        std::vector<Tile> tiles,
        isize width,
        isize height,
        Vector start,
        Vector end
    )
        : _tiles(std::move(tiles)),
          _width(width),
          _height(height),
          _start(start),
          _end(end) {
        assert(_width >= 0);
        assert(_height >= 0);
        assert(std::ssize(_tiles) == (_width * _height));
        assert(exists(_start, _width, _height));
        assert(exists(_end, _width, _height));
    }

    /**
     * @brief Determines whether a position exists on a map with the specified
     * dimensions.
     *
     * @warning The behavior is undefined if `width` or `height` is negative.
     *
     * @param[in] pos    The position to check.
     * @param[in] width  The width of the map.
     * @param[in] height The height of the map.
     * @return `true` if the position exists on a map with the specified
     * dimensions, otherwise `false`.
     */
    static bool exists(Vector pos, isize width, isize height) noexcept {
        assert(width >= 0);
        assert(height >= 0);
        return (pos.x >= 0) && (pos.x < width)
            && (pos.y >= 0) && (pos.y < height);
    }

    /**
     * @brief Returns the position that results from moving one step in a
     * specified direction.
     *
     * @param[in] pos The current position.
     * @param[in] dir The direction to move towards.
     * @return The position that results from moving one step in the specified
     * direction.
     */
    static Vector move(Vector pos, Direction dir) noexcept {
        switch (dir) {
            case Direction::West:
                return { .x = pos.x - 1, .y = pos.y };
            case Direction::North:
                return { .x = pos.x, .y = pos.y - 1 };
            case Direction::East:
                return { .x = pos.x + 1, .y = pos.y };
            case Direction::South:
                return { .x = pos.x, .y = pos.y + 1 };
            default:
                std::unreachable();
        }
    }

    /**
     * @brief Determines whether a tile at a specified position is of a
     * specified type.
     *
     * @note If `pos` is outside the bounds of the map, the function returns
     * `false`.
     *
     * @param[in] pos  The position to check.
     * @param[in] tile The type of tile to check for.
     * @return `true` if the tile at the specified position is of the specified
     * type, otherwise `false`.
     */
    bool is_tile(Vector pos, Tile tile) const noexcept {
        return exists(pos, _width, _height)
            && (_tiles[pos.y * _width + pos.x] == tile);
    }

    /**
     * @brief Returns the next possible states for a specified state.
     *
     * @param[in] state The current state.
     * @return The next possible states for the specified state.
     */
    std::generator<std::pair<State, isize>> next_states(
        const State& state
    ) const noexcept {
        const auto& [pos, dir] = state;
        Vector prev;
        Vector next = pos;
        Direction left;
        Direction right;
        switch (dir) {
            case Direction::West:
                left = Direction::South;
                right = Direction::North;
                break;
            case Direction::North:
                left = Direction::West;
                right = Direction::East;
                break;
            case Direction::East:
                left = Direction::North;
                right = Direction::South;
                break;
            case Direction::South:
                left = Direction::East;
                right = Direction::West;
                break;
            default:
                std::unreachable();
        }
        auto distance = [](Vector lhs, Vector rhs) noexcept -> isize {
            return std::abs(lhs.x - rhs.x) + std::abs(lhs.y - rhs.y);
        };
        while (is_tile(next, Tile::Empty)) {
            if (is_tile(move(next, left), Tile::Empty)) {
                co_yield std::make_pair<State, isize>(
                    { .pos = next, .dir = left },
                    distance(pos, next) + 1000
                );
            }
            if (is_tile(move(next, right), Tile::Empty)) {
                co_yield std::make_pair<State, isize>(
                    { .pos = next, .dir = right },
                    distance(pos, next) + 1000
                );
            }
            prev = next;
            next = move(next, dir);
        }
        co_yield std::make_pair<State, isize>(
            { .pos = prev, .dir = dir },
            distance(pos, prev)
        );
    }

    /**
     * @brief Finds the best spots on this map based on the specified scores and
     * predecessor states.
     *
     * @param[in] scores       A mapping of states to their corresponding
     *                         scores.
     * @param[in] predecessors A mapping of states to their predecessor states.
     * @return The best spots on this map.
     */
    std::unordered_set<Vector> find_best_spots(
        const std::unordered_map<State, isize>& scores,
        const std::unordered_map<State, std::vector<State>>& predecessors
    ) const {
        std::array<Direction, 4> dirs = {
            Direction::West,
            Direction::North,
            Direction::East,
            Direction::South
        };
        isize lowestScore = std::numeric_limits<isize>::max();
        for (Direction dir : dirs) {
            State end = { .pos = _end, .dir = dir };
            auto it = scores.find(end);
            if (it != scores.end()) {
                lowestScore = std::min(lowestScore, it->second);
            }
        }
        std::unordered_set<Vector> bestSpots;
        std::unordered_set<State> visited;
        std::stack<State> stack;
        for (Direction dir : dirs) {
            State end = { .pos = _end, .dir = dir };
            auto it = scores.find(end);
            if ((it != scores.end()) && (it->second == lowestScore)) {
                visited.insert(end);
                stack.push(end);
            }
        }
        while (!stack.empty()) {
            State state = stack.top();
            stack.pop();
            bestSpots.insert(state.pos);
            auto it = predecessors.find(state);
            if (it == predecessors.end()) {
                continue;
            }
            for (const State& pred : it->second) {
                for (
                    Vector pos = pred.pos;
                    pos != state.pos;
                    pos = move(pos, pred.dir)
                ) {
                    bestSpots.insert(pos);
                }
                const auto [_, inserted] = visited.insert(pred);
                if (inserted) {
                    stack.push(pred);
                }
            }
        }
        return bestSpots;
    }

public:

    /**
     * Parses a map from the standard input stream.
     *
     * The input is expected to consist of one or more lines of the same length,
     * where each character must be either `.` (for `Tile::Empty`) or `#` (for
     * `Tile::Wall`). Additionally, the input must contain exactly one `S`
     * (marking the starting position of the reindeer) and exactly one `E`
     * (marking the ending position of the reindeer).
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * ###############
     * #.......#....E#
     * #.#.###.#.###.#
     * #.....#.#...#.#
     * #.###.#####.#.#
     * #.#.#.......#.#
     * #.#.#####.###.#
     * #...........#.#
     * ###.#.#####.#.#
     * #...#.....#.#.#
     * #.#.#.###.#.#.#
     * #.....#...#.#.#
     * #.###.#.#.#.#.#
     * #S..#.....#...#
     * ###############
     * ```
     *
     * @return The parsed map on success, or `std::nullopt` on failure.
     */
    static std::optional<Map> parse() {
        std::vector<Tile> tiles;
        isize width = 0;
        isize height = 0;
        Vector start = { };
        Vector end = { };
        bool foundWidth = false;
        bool foundStart = false;
        bool foundEnd = false;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!foundWidth) {
                width = std::ssize(line);
                foundWidth = true;
            }
            if (std::ssize(line) != width) {
                return std::nullopt;
            }
            for (isize i = 0; i < width; i++) {
                switch (line[i]) {
                    case '.':
                        tiles.push_back(Tile::Empty);
                        break;
                    case '#':
                        tiles.push_back(Tile::Wall);
                        break;
                    case 'S':
                        if (foundStart) {
                            return std::nullopt;
                        }
                        tiles.push_back(Tile::Empty);
                        start = { .x = i, .y = height };
                        foundStart = true;
                        break;
                    case 'E':
                        if (foundEnd) {
                            return std::nullopt;
                        }
                        tiles.push_back(Tile::Empty);
                        end = { .x = i, .y = height };
                        foundEnd = true;
                        break;
                    default:
                        return std::nullopt;
                }
            }
            height++;
        }
        if (!foundWidth || !foundStart || !foundEnd) {
            return std::nullopt;
        }
        return Map(std::move(tiles), width, height, start, end);
    }

    /**
     * Returns the lowest score a reindeer could get on this map.
     *
     * @return The lowest score a reindeer could get on this map.
     */
    isize lowest_score() const {
        State state = { .pos = _start, .dir = Direction::East };
        std::unordered_map<State, isize> scores;
        scores.insert({ state, 0 });
        auto compare_states = [&scores](
            const State& lhs,
            const State& rhs
        ) -> bool {
            return scores[lhs] > scores[rhs];
        };
        std::priority_queue<
            State,
            std::vector<State>,
            decltype(compare_states)
        > queue(compare_states);
        queue.push(state);
        while (!queue.empty()) {
            state = queue.top();
            queue.pop();
            if (state.pos == _end) {
                return scores[state];
            }
            for (const auto& [next, score] : next_states(state)) {
                isize nextScore = scores[state] + score;
                if (!scores.contains(next) || (nextScore < scores[next])) {
                    scores[next] = nextScore;
                    queue.push(next);
                }
            }
        }
        std::unreachable();
    }

    /**
     * @brief Returns the number of best spots on this map.
     *
     * A best spot is defined as a position which is part of at least one of the
     * best paths through the reindeer maze.
     *
     * @return The number of best spots on this map.
     */
    isize best_spots() const {
        State state = { .pos = _start, .dir = Direction::East };
        std::unordered_map<State, isize> scores;
        scores.insert({ state, 0 });
        auto compare_states = [&scores](
            const State& lhs,
            const State& rhs
        ) -> bool {
            return scores[lhs] > scores[rhs];
        };
        std::priority_queue<
            State,
            std::vector<State>,
            decltype(compare_states)
        > queue(compare_states);
        queue.push(state);
        std::unordered_map<State, std::vector<State>> predecessors;
        while (!queue.empty()) {
            state = queue.top();
            queue.pop();
            for (const auto& [next, score] : next_states(state)) {
                isize nextScore = scores[state] + score;
                if (!scores.contains(next) || (nextScore < scores[next])) {
                    scores[next] = nextScore;
                    queue.push(next);
                    predecessors[next] = { state };
                }
                else if (nextScore == scores[next]) {
                    predecessors[next].push_back(state);
                }
            }
        }
        return std::ssize(find_best_spots(scores, predecessors));
    }

};

int main() {
    std::optional<Map> map = Map::parse();
    if (!map) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize lowestScore = map->lowest_score();
    isize bestSpots = map->best_spots();
    std::println("The lowest score a reindeer could get is {}.", lowestScore);
    std::println("The number of best spots on the map is {}.", bestSpots);
    return EXIT_SUCCESS;
}