#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <generator>
#include <iostream>
#include <optional>
#include <print>
#include <queue>
#include <span>
#include <string>
#include <utility>
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
     * @param[in] a The first position.
     * @param[in] b The second position.
     * @return `true` if the positions are equal, otherwise `false`.
     */
    friend bool operator==(const Position& a, const Position& b) noexcept {
        return (a.x == b.x) && (a.y == b.y);
    }

};

/** @brief Represents a grid of the memory space. */
class Grid final {
public:

    /** @brief The width of this grid. */
    static constexpr isize WIDTH = 71;

    /** @brief The height of this grid. */
    static constexpr isize HEIGHT = 71;

private:

    /** @brief Represents a tile in the grid. */
    enum class Tile {
        None,
        Byte
    };

    /** @brief The starting position of the search for the exit. */
    static constexpr Position START = { .x = 0, .y = 0 };

    /** @brief The ending position of the search for the exit. */
    static constexpr Position END = { .x = WIDTH - 1, .y = HEIGHT - 1 };

    /** @brief The tiles of this grid. */
    std::array<Tile, WIDTH * HEIGHT> _tiles;

    /**
     * @brief Determines whether a position exists in this grid.
     *
     * @param[in] pos The position to check.
     * @return `true` if the position exists in this grid, otherwise `false`.
     */
    bool exists(const Position& pos) const noexcept {
        return (pos.x >= 0) && (pos.x < WIDTH)
            && (pos.y >= 0) && (pos.y < HEIGHT);
    }

    /**
     * @brief Returns the accessible neighbors of a position in this grid.
     *
     * @param[in] pos The position to get the accessible neighbors of.
     * @return The accessible neighbors of the specified position.
     */
    std::generator<Position> accessible_neighbors(
        const Position& pos
    ) const noexcept {
        auto is_accessible = [this](const Position& p) -> bool {
            return exists(p) && (_tiles[p.y * WIDTH + p.x] == Tile::None);
        };
        assert(is_accessible(pos));
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

public:

    /** @brief Initializes a new empty grid. */
    Grid() noexcept : _tiles({ }) { }

    /**
     * @brief Adds falling bytes to this grid.
     *
     * @warning The behavior is undefined if any of the specified positions does
     * not exist in this grid.
     *
     * @param[in] bytes The positions of the falling bytes to add.
     */
    void add_bytes(std::span<const Position> bytes) noexcept {
        for (const Position& pos : bytes) {
            assert(exists(pos));
            _tiles[pos.y * WIDTH + pos.x] = Tile::Byte;
        }
    }

    /**
     * @brief Returns the minimum number of steps needed to reach the exit.
     *
     * The search for the exit starts at the top-left corner of the grid (i.e.,
     * at `X = 0, Y = 0`) and ends at the bottom-right corner of the grid (i.e.,
     * at `X = WIDTH - 1, Y = HEIGHT - 1`).
     *
     * @return The minimum number of steps needed to reach the exit, or `-1` if
     * the exit is unreachable.
     */
    isize min_steps() const {
        std::array<bool, WIDTH * HEIGHT> visited = { };
        visited[START.y * WIDTH + START.x] = true;
        using State = std::pair<Position, isize>;
        std::queue<State> queue;
        queue.push({ START, 0 });
        while (!queue.empty()) {
            auto [position, steps] = queue.front();
            queue.pop();
            if (position == END) {
                return steps;
            }
            for (const Position& neighbor : accessible_neighbors(position)) {
                if (!visited[neighbor.y * WIDTH + neighbor.x]) {
                    visited[neighbor.y * WIDTH + neighbor.x] = true;
                    queue.push({ neighbor, steps + 1 });
                }
            }
        }
        return -1;
    }

    /**
     * @brief Returns the position of the first blocking byte in this grid.
     *
     * A blocking byte is a falling byte that prevents the search for the exit
     * from reaching the bottom-right corner of the grid (i.e., at `X = WIDTH -
     * 1, Y = HEIGHT - 1`).
     *
     * @warning The behavior is undefined if any of the specified positions does
     * not exist in this grid, or if the exit does not become unreachable even
     * after adding all of the specified falling bytes to this grid.
     *
     * @param[in] bytes The positions of the falling bytes.
     * @return The position of the first blocking byte.
     */
    Position first_blocking_byte(std::span<const Position> bytes) const {
        isize low = 0;
        isize high = std::ssize(bytes) - 1;
        while (low <= high) {
            isize mid = low + (high - low) / 2;
            Grid copy = *this;
            copy.add_bytes(std::span<const Position>(bytes.begin(), mid + 1));
            if (copy.min_steps() == -1) {
                high = mid - 1;
            }
            else {
                low = mid + 1;
            }
        }
        return bytes[low];
    }

};

/**
 * @brief Parses the positions of the falling bytes from the standard input
 * stream.
 *
 * The input must consist of zero or more lines with the following format:
 *
 * ```plaintext
 * <x>,<y>
 * ```
 *
 * Here, `<x>` and `<y>` represent the x- and y-coordinates of a falling byte.
 * The coordinates must be between `0` and `Grid::WIDTH - 1` and `0` and
 * `Grid::HEIGHT - 1`, respectively.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * 5,4
 * 4,2
 * 4,5
 * 3,0
 * 2,1
 * ```
 *
 * @return The parsed positions of the falling bytes on success, or
 * `std::nullopt` on failure.
 */
static std::optional<std::vector<Position>> parse_bytes() {
    std::vector<Position> bytes;
    std::string line;
    while (std::getline(std::cin, line)) {
        Position pos;
        if (
            (std::sscanf(line.c_str(), "%td,%td", &pos.x, &pos.y) != 2)
                || (pos.x < 0) || (pos.x >= Grid::WIDTH)
                || (pos.y < 0) || (pos.y >= Grid::HEIGHT)
        ) {
            return std::nullopt;
        }
        bytes.push_back(pos);
    }
    return bytes;
}

int main() {
    std::optional<std::vector<Position>> bytes = parse_bytes();
    if (!bytes) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    Grid grid;
    grid.add_bytes(std::span<const Position>(bytes->begin(), 1024));
    isize minSteps = grid.min_steps();
    Position firstBlocking = grid.first_blocking_byte(
        std::span<const Position>(bytes->begin() + 1024, bytes->end())
    );
    std::println(
        "The minimum number of steps needed to reach the exit is {}.",
        minSteps
    );
    std::println(
        "The first blocking byte is at {},{}.",
        firstBlocking.x,
        firstBlocking.y
    );
    return EXIT_SUCCESS;
}