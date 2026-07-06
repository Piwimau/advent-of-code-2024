#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <span>
#include <unordered_set>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a tile in a grid. */
enum class Tile {
    None,
    Obstacle
};

/** @brief Represents a direction in which a guard can move. */
enum class Direction {
    Up,
    Right,
    Down,
    Left
};

/** @brief Represents a two-dimensional position. */
struct Position {

    /** @brief The x-coordinate of this position. */
    isize x;

    /** @brief The y-coordinate of this position. */
    isize y;

    /** @brief Initializes a new position with default coordinates. */
    Position() : x(0), y(0) { }

    /**
     * @brief Initializes a new position with the specified coordinates.
     *
     * @param[in] x The x-coordinate of the position.
     * @param[in] y The y-coordinate of the position.
     */
    Position(isize x, isize y) : x(x), y(y) { }

    /**
     * @brief Determines whether this position is equal to a specified position.
     *
     * @param[in] other The other position.
     * @return `true` if this position is equal to the specified position,
     * otherwise `false`.
     */
    bool operator==(const Position& other) const noexcept {
        return (x == other.x) && (y == other.y);
    }

};

template<>
struct std::hash<Position> {
    usize operator()(const Position& position) const noexcept {
        usize hash = std::hash<isize>()(position.x);
        hash ^= std::hash<isize>()(position.y) + 0x9E3779B9 + (hash << 6)
            + (hash >> 2);
        return hash;
    }
};

/** @brief Represents a two-dimensional grid of tiles in a lab. */
class Grid final {
private:

    /**
     * @brief The indices of columns with obstacles in each row of this grid.
     */
    std::vector<std::vector<isize>> _obstaclesInRow;

    /**
     * @brief The indices of rows with obstacles in each column of this grid.
     */
    std::vector<std::vector<isize>> _obstaclesInCol;

    /** @brief The width of this grid. */
    isize _width;

    /** @brief The height of this grid. */
    isize _height;

    /** @brief The position of the guard in this grid. */
    Position _guard;

    /**
     * @brief Initializes a new grid with the specified tiles, width, height,
     * and guard position.
     *
     * @warning The behavior is undefined if `tiles` does not contain exactly
     * `width * height` tiles, if `width` or `height` is negative, or if the
     * guard's position does not exist in the grid.
     *
     * @param[in] tiles  The tiles of the grid.
     * @param[in] width  The width of the grid.
     * @param[in] height The height of the grid.
     * @param[in] guard  The position of the guard in the grid.
     */
    Grid(std::span<const Tile> tiles, isize width, isize height, Position guard)
        : _obstaclesInRow(height),
          _obstaclesInCol(width),
          _width(width),
          _height(height),
          _guard(guard) {
        assert(_width >= 0);
        assert(_height >= 0);
        assert(std::ssize(tiles) == (_width * _height));
        assert(
            (guard.x >= 0) && (guard.x < _width)
                && (guard.y >= 0) && (guard.y < _height)
        );
        for (isize y = 0; y < _height; y++) {
            for (isize x = 0; x < _width; x++) {
                Position position(x, y);
                if (tiles[y * _width + x] == Tile::Obstacle) {
                    _obstaclesInRow[y].push_back(x);
                    _obstaclesInCol[x].push_back(y);
                }
            }
        }
    }

    /**
     * @brief Returns the distinct positions visited by the guard.
     *
     * The guard starts at `guard` and initially faces up. The guard moves
     * forward one step at a time, but turns right whenever it encounters an
     * obstacle. The guard continues moving until it leaves the area of the
     * grid.
     *
     * @return The distinct positions visited by the guard.
     */
    std::unordered_set<Position> visited_positions() const {
        std::unordered_set<Position> visited;
        Position cur = _guard;
        Direction dir = Direction::Up;
        while (true) {
            bool exits = false;
            switch (dir) {
                case Direction::Up: {
                    const auto& obstacles = _obstaclesInCol[cur.x];
                    auto it = std::ranges::lower_bound(obstacles, cur.y);
                    isize stopY = (it != obstacles.begin())
                        ? *std::prev(it) + 1
                        : 0;
                    exits = (it == obstacles.begin());
                    for (isize y = stopY; y <= cur.y; y++) {
                        visited.emplace(cur.x, y);
                    }
                    cur.y = stopY;
                    dir = Direction::Right;
                    break;
                }
                case Direction::Right: {
                    const auto& obstacles = _obstaclesInRow[cur.y];
                    auto it = std::ranges::upper_bound(obstacles, cur.x);
                    isize stopX = (it != obstacles.end())
                        ? *it - 1
                        : _width - 1;
                    exits = (it == obstacles.end());
                    for (isize x = cur.x; x <= stopX; x++) {
                        visited.emplace(x, cur.y);
                    }
                    cur.x = stopX;
                    dir = Direction::Down;
                    break;
                }
                case Direction::Down: {
                    const auto& obstacles = _obstaclesInCol[cur.x];
                    auto it = std::ranges::upper_bound(obstacles, cur.y);
                    isize stopY = (it != obstacles.end())
                        ? *it - 1
                        : _height - 1;
                    exits = (it == obstacles.end());
                    for (isize y = cur.y; y <= stopY; y++) {
                        visited.emplace(cur.x, y);
                    }
                    cur.y = stopY;
                    dir = Direction::Left;
                    break;
                }
                case Direction::Left: {
                    const auto& obstacles = _obstaclesInRow[cur.y];
                    auto it = std::ranges::lower_bound(obstacles, cur.x);
                    isize stopX = (it != obstacles.begin())
                        ? *std::prev(it) + 1
                        : 0;
                    exits = (it == obstacles.begin());
                    for (isize x = stopX; x <= cur.x; x++) {
                        visited.emplace(x, cur.y);
                    }
                    cur.x = stopX;
                    dir = Direction::Up;
                    break;
                }
                default:
                    std::unreachable();
            }
            if (exits) {
                break;
            }
        }
        return visited;
    }

public:

    /**
     * @brief Parses a grid from the standard input stream.
     *
     * The input is expected to consist of zero or more lines of equal length,
     * where each line represents a row of tiles in the grid. Each character in
     * the input must be either `'.'` (`Tile::None`) or `'#'`
     * (`Tile::Obstacle`). Additionally, there must be exactly one `'^'`
     * character representing the guard's starting position, which is considered
     * to be a `Tile::None` tile. The individual lines must be separated with a
     * newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * ....#.....
     * .........#
     * ..........
     * ..#.......
     * .......#..
     * ..........
     * .#..^.....
     * ........#.
     * #.........
     * ......#...
     *
     * @return The parsed grid on success, or `std::nullopt` on failure.
     */
    static std::optional<Grid> parse() {
        std::vector<Tile> tiles;
        isize width = 0;
        isize height = 0;
        Position guard;
        bool foundWidth = false;
        bool foundGuard = false;
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
                switch (c) {
                    case '.':
                        tiles.push_back(Tile::None);
                        break;
                    case '#':
                        tiles.push_back(Tile::Obstacle);
                        break;
                    case '^':
                        if (foundGuard) {
                            return std::nullopt;
                        }
                        tiles.push_back(Tile::None);
                        guard = Position(
                            std::ssize(tiles) % width - 1,
                            height
                        );
                        foundGuard = true;
                        break;
                    default:
                        return std::nullopt;
                }
            }
            height++;
        }
        if (!foundGuard) {
            return std::nullopt;
        }
        return Grid(tiles, width, height, guard);
    }

    /**
     * @brief Returns the number of distinct positions visited by the guard.
     *
     * The guard starts at `guard` and initially faces up. The guard moves
     * forward one step at a time, but turns right whenever it encounters an
     * obstacle. The guard continues moving until it leaves the area of the
     * grid.
     *
     * @return The number of distinct positions visited by the guard.
     */
    isize count_visited_positions() const {
        return std::ssize(visited_positions());
    }

    /**
     * @brief Returns the number of possible positions in which an obstacle
     * could be placed to get the guard stuck in an infinite loop.
     *
     * @return The number of possible positions in which an obstacle could be
     * placed to get the guard stuck in an infinite loop.
     */
    isize possible_obstructions() const {
        std::unordered_set<Position> obstacles = visited_positions();
        obstacles.erase(_guard);
        std::vector<isize> visited(_width * _height * 4, 0);
        isize obstructions = 0;
        isize round = 0;
        for (const Position& obstacle : obstacles) {
            round++;
            Position cur = _guard;
            Direction dir = Direction::Up;
            while (true) {
                isize idx = (cur.y * _width + cur.x) * 4
                    + std::to_underlying(dir);
                if (visited[idx] == round) {
                    obstructions++;
                    break;
                }
                visited[idx] = round;
                bool exits = false;
                switch (dir) {
                    case Direction::Up: {
                        const auto& col = _obstaclesInCol[cur.x];
                        auto it = std::ranges::lower_bound(col, cur.y);
                        isize wall = (it != col.begin()) ? *std::prev(it) : -1;
                        if ((obstacle.x == cur.x) && (obstacle.y < cur.y)) {
                            wall = std::max(wall, obstacle.y);
                        }
                        if (wall < 0) {
                            exits = true;
                            break;
                        }
                        cur.y = wall + 1;
                        dir = Direction::Right;
                        break;
                    }
                    case Direction::Right: {
                        const auto& row = _obstaclesInRow[cur.y];
                        auto it = std::ranges::upper_bound(row, cur.x);
                        isize wall = (it != row.end()) ? *it : _width;
                        if ((obstacle.y == cur.y) && (obstacle.x > cur.x)) {
                            wall = std::min(wall, obstacle.x);
                        }
                        if (wall >= _width) {
                            exits = true;
                            break;
                        }
                        cur.x = wall - 1;
                        dir = Direction::Down;
                        break;
                    }
                    case Direction::Down: {
                        const auto& col = _obstaclesInCol[cur.x];
                        auto it = std::ranges::upper_bound(col, cur.y);
                        isize wall = (it != col.end()) ? *it : _height;
                        if ((obstacle.x == cur.x) && (obstacle.y > cur.y)) {
                            wall = std::min(wall, obstacle.y);
                        }
                        if (wall >= _height) {
                            exits = true;
                            break;
                        }
                        cur.y = wall - 1;
                        dir = Direction::Left;
                        break;
                    }
                    case Direction::Left: {
                        const auto& row = _obstaclesInRow[cur.y];
                        auto it = std::ranges::lower_bound(row, cur.x);
                        isize wall = (it != row.begin()) ? *std::prev(it) : -1;
                        if ((obstacle.y == cur.y) && (obstacle.x < cur.x)) {
                            wall = std::max(wall, obstacle.x);
                        }
                        if (wall < 0) {
                            exits = true;
                            break;
                        }
                        cur.x = wall + 1;
                        dir = Direction::Up;
                        break;
                    }
                    default:
                        std::unreachable();
                }
                if (exits) {
                    break;
                }
            }
        }
        return obstructions;
    }

};

int main() {
    std::optional<Grid> grid = Grid::parse();
    if (!grid) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize visited = grid->count_visited_positions();
    isize obstructions = grid->possible_obstructions();
    std::println("The guard will visit {} distinct positions.", visited);
    std::println(
        "There are {} possible positions for obstacles to get the guard stuck.",
        obstructions
    );
    return EXIT_SUCCESS;
}