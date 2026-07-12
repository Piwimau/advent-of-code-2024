#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <optional>
#include <print>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a warehouse of boxes and a robot. */
class Warehouse final {
private:

    /** @brief Represents a tile in the warehouse. */
    enum class Tile {
        None,
        Wall,
        Box,
        BoxLeft,
        BoxRight,
        Robot
    };

    /** @brief Represents a direction for moving in the warehouse. */
    enum class Move {
        Left,
        Up,
        Right,
        Down
    };

    /** @brief Represents a two-dimensional vector. */
    struct Vector {

        /** @brief The x-component of the vector. */
        isize x;

        /** @brief The y-component of the vector. */
        isize y;

        /**
         * @brief Adds a specified vector to this vector.
         *
         * @param[in] rhs The vector to add to this vector.
         * @return A reference to this vector.
         */
        Vector& operator+=(const Vector& rhs) noexcept {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        /**
         * @brief Adds two specified vectors.
         *
         * @param[in] lhs The first vector.
         * @param[in] rhs The second vector.
         * @return The result of adding the two specified vectors.
         */
        friend Vector operator+(Vector lhs, const Vector& rhs) noexcept {
            return lhs += rhs;
        }

        /**
         * @brief Determines whether two specified vectors are equal.
         *
         * @param[in] lhs The first vector.
         * @param[in] rhs The second vector.
         * @return `true` if the two specified vectors are equal, otherwise
         * `false`.
         */
        friend bool operator==(const Vector& lhs, const Vector& rhs) noexcept {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y);
        }

    };

    /** @brief The tiles of the warehouse. */
    std::vector<Tile> _tiles;

    /** @brief The width of the warehouse. */
    isize _width;

    /** @brief The height of the warehouse. */
    isize _height;

    /** @brief The position of the robot in the warehouse. */
    Vector _robot;

    /** @brief The moves the robot will make. */
    std::vector<Move> _moves;

    /**
     * @brief Initializes a new warehouse with the specified tiles, width, and
     * height.
     *
     * @warning The behavior is undefined if `tiles` does not have `width *
     * height` elements, if `width` or `height` is negative, or if `robot` is
     * outside the bounds of the warehouse.
     *
     * @param[in] tiles  The tiles of the warehouse.
     * @param[in] width  The width of the warehouse.
     * @param[in] height The height of the warehouse.
     * @param[in] robot  The position of the robot in the warehouse.
     * @param[in] moves  The moves the robot will make.
     */
    Warehouse(
        std::vector<Tile> tiles,
        isize width,
        isize height,
        Vector robot,
        std::vector<Move> moves
    )
        : _tiles(std::move(tiles)),
          _width(width),
          _height(height),
          _robot(robot),
          _moves(std::move(moves)) {
        assert(_width >= 0);
        assert(_height >= 0);
        assert(std::ssize(_tiles) == (_width * _height));
        assert(exists(_robot, _width, _height));
    }

    /**
     * @brief Returns the offset vector for a specified move.
     *
     * @param[in] move The move to get the offset vector for.
     * @return The offset vector for the specified move.
     */
    static Vector offset(Move move) noexcept {
        switch (move) {
            case Move::Left:
                return { .x = -1, .y = 0 };
            case Move::Up:
                return { .x = 0, .y = -1 };
            case Move::Right:
                return { .x = 1, .y = 0 };
            case Move::Down:
                return { .x = 0, .y = 1 };
            default:
                std::unreachable();
        }
    }

    /**
     * @brief Determines whether a specified position exists in a warehouse with
     * the specified dimensions.
     *
     * @warning The behavior is undefined if `width` or `height` is negative.
     *
     * @param[in] pos    The position to check.
     * @param[in] width  The width of the warehouse.
     * @param[in] height The height of the warehouse.
     * @return `true` if the specified position exists in the warehouse with the
     * specified dimensions, otherwise `false`.
     */
    static bool exists(Vector pos, isize width, isize height) noexcept {
        assert(width >= 0);
        assert(height >= 0);
        return (pos.x >= 0) && (pos.x < width)
            && (pos.y >= 0) && (pos.y < height);
    }

public:

    /**
     * @brief Parses a warehouse from the standard input stream.
     *
     * The input is expected to consist of one or more lines in the following
     * format:
     *
     * ```plaintext
     * <warehouse-map>
     *
     * <moves>
     * ```
     *
     * Here, `<warehouse-map>` represents the initial state of the warehouse. It
     * must consist of zero or more lines of the same length, where each
     * character must be either `.`, `#`, `O`, or `@`. The character `.`
     * represents an empty tile, `#` represents a wall, `O` represents a box,
     * and `@` represents the robot. There must be exactly one `@` character in
     * the entire `<warehouse-map>`.
     *
     * The `<moves>` section represents the moves the robot will make. It must
     * be preceded by a blank line, and must consist of zero or more lines of
     * characters, where each character must be either `<`, `^`, `>`, or `v`
     * (representing left, up, right, and down, respectively). Any newline
     * characters used to separate the lines of `<moves>` (if any) are ignored
     * by the parser (i.e., it treates the `<moves>` section as if it was
     * specified on a single line).
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * ########
     * #..O.O.#
     * ##@.O..#
     * #...O..#
     * #.#.O..#
     * #...O..#
     * #......#
     * ########
     *
     * <^^>>>vv<v>>v<<
     * ```
     *
     * @return The parsed warehouse on success, or `std::nullopt` on failure.
     */
    static std::optional<Warehouse> parse() {
        std::vector<Tile> tiles;
        isize width = 0;
        isize height = 0;
        Vector robot = { };
        bool foundWidth = false;
        bool foundRobot = false;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!foundWidth) {
                width = std::ssize(line);
                foundWidth = true;
            }
            if (line.empty()) {
                break;
            }
            if (std::ssize(line) != width) {
                return std::nullopt;
            }
            for (isize i = 0; i < width; i++) {
                switch (line[i]) {
                    case '.':
                        tiles.push_back(Tile::None);
                        break;
                    case '#':
                        tiles.push_back(Tile::Wall);
                        break;
                    case 'O':
                        tiles.push_back(Tile::Box);
                        break;
                    case '@':
                        if (foundRobot) {
                            return std::nullopt;
                        }
                        tiles.push_back(Tile::Robot);
                        robot = { .x = i, .y = height };
                        foundRobot = true;
                        break;
                    default:
                        return std::nullopt;
                }
            }
            height++;
        }
        if (!foundWidth || !foundRobot) {
            return std::nullopt;
        }
        std::vector<Move> moves;
        while (std::getline(std::cin, line)) {
            for (char c : line) {
                switch (c) {
                    case '<':
                        moves.push_back(Move::Left);
                        break;
                    case '^':
                        moves.push_back(Move::Up);
                        break;
                    case '>':
                        moves.push_back(Move::Right);
                        break;
                    case 'v':
                        moves.push_back(Move::Down);
                        break;
                    default:
                        return std::nullopt;
                }
            }
        }
        return Warehouse(
            std::move(tiles),
            width,
            height,
            robot,
            std::move(moves)
        );
    }

    /**
     * @brief Returns the sum of all boxes' Goods Positioning System (GPS)
     * coordinates after simulating all the robot's moves.
     *
     * The GPS coordinates of a box are defined as `100 * y + x`, where `x` and
     * `y` represent the box's distance from the left and top edges of the
     * warehouse, respectively.
     *
     * @return The sum of all boxes' GPS coordinates.
     */
    isize sum_of_gps_coordinates() const {
        std::vector<Tile> tiles = _tiles;
        Vector robot = _robot;
        for (Move move : _moves) {
            Vector dir = offset(move);
            Vector pos = robot + dir;
            while (
                exists(pos, _width, _height)
                    && (tiles[pos.y * _width + pos.x] == Tile::Box)
            ) {
                pos += dir;
            }
            if (
                exists(pos, _width, _height)
                    && (tiles[pos.y * _width + pos.x] == Tile::None)
            ) {
                Vector next = robot + dir;
                std::swap(
                    tiles[next.y * _width + next.x],
                    tiles[pos.y * _width + pos.x]
                );
                std::swap(
                    tiles[robot.y * _width + robot.x],
                    tiles[next.y * _width + next.x]
                );
                robot += dir;
            }
        }
        isize sum = 0;
        for (isize y = 0; y < _height; y++) {
            for (isize x = 0; x < _width; x++) {
                if (tiles[y * _width + x] == Tile::Box) {
                    sum += 100 * y + x;
                }
            }
        }
        return sum;
    }

    /**
     * @brief Returns the sum of all boxes' Goods Positioning System (GPS)
     * coordinates after simulating all the robot's moves in a scaled-up version
     * of the warehouse.
     *
     * The GPS coordinates of a box are defined as `100 * y + x`, where `x` and
     * `y` represent the box's distance from the left and top edges of the
     * warehouse, respectively.
     *
     * @return The sum of all boxes' GPS coordinates in the scaled-up warehouse.
     */
    isize sum_of_gps_coordinates_scaled() const {
        isize newWidth = _width * 2;
        std::vector<Tile> tiles(newWidth * _height);
        for (isize i = 0; i < std::ssize(_tiles); i++) {
            switch (_tiles[i]) {
                case Tile::None:
                    tiles[i * 2] = Tile::None;
                    tiles[i * 2 + 1] = Tile::None;
                    break;
                case Tile::Wall:
                    tiles[i * 2] = Tile::Wall;
                    tiles[i * 2 + 1] = Tile::Wall;
                    break;
                case Tile::Box:
                    tiles[i * 2] = Tile::BoxLeft;
                    tiles[i * 2 + 1] = Tile::BoxRight;
                    break;
                case Tile::Robot:
                    tiles[i * 2] = Tile::Robot;
                    tiles[i * 2 + 1] = Tile::None;
                    break;
                default:
                    std::unreachable();
            }
        }
        Vector robot = { .x = _robot.x * 2, .y = _robot.y };
        std::vector<Vector> toMove;
        std::deque<Vector> queue;
        for (Move move : _moves) {
            Vector dir = offset(move);
            if ((move == Move::Left) || (move == Move::Right)) {
                Vector pos = robot + dir;
                while (
                    exists(pos, newWidth, _height)
                        && ((tiles[pos.y * newWidth + pos.x] == Tile::BoxLeft)
                            || (tiles[pos.y * newWidth + pos.x] == Tile::BoxRight))
                ) {
                    pos += dir;
                }
                if (
                    exists(pos, newWidth, _height)
                        && (tiles[pos.y * newWidth + pos.x] == Tile::None)
                ) {
                    if (move == Move::Left) {
                        std::ranges::copy(
                            &tiles[pos.y * newWidth + pos.x + 1],
                            &tiles[robot.y * newWidth + robot.x + 1],
                            &tiles[pos.y * newWidth + pos.x]
                        );
                    }
                    else {
                        std::ranges::copy_backward(
                            &tiles[robot.y * newWidth + robot.x],
                            &tiles[pos.y * newWidth + pos.x],
                            &tiles[pos.y * newWidth + pos.x + 1]
                        );
                    }
                    tiles[robot.y * newWidth + robot.x] = Tile::None;
                    robot += dir;
                }
            }
            else {
                toMove.clear();
                queue.clear();
                queue.push_back(robot);
                bool isBlocked = false;
                while (!queue.empty()) {
                    Vector pos = queue.front();
                    queue.pop_front();
                    if (!std::ranges::contains(toMove, pos)) {
                        toMove.push_back(pos);
                    }
                    Vector next = pos + dir;
                    if (exists(next, newWidth, _height)) {
                        switch (tiles[next.y * newWidth + next.x]) {
                            case Tile::Wall:
                                isBlocked = true;
                                goto move;
                            case Tile::BoxLeft:
                                queue.push_back(next);
                                queue.push_back(next + offset(Move::Right));
                                break;
                            case Tile::BoxRight:
                                queue.push_back(next);
                                queue.push_back(next + offset(Move::Left));
                                break;
                            default:
                                break;
                        }
                    }
                }
            move:
                if (!isBlocked) {
                    if (move == Move::Up) {
                        std::ranges::sort(toMove, std::less<>(), &Vector::y);
                    }
                    else {
                        std::ranges::sort(toMove, std::greater<>(), &Vector::y);
                    }
                    for (Vector pos : toMove) {
                        Vector next = pos + dir;
                        std::swap(
                            tiles[pos.y * newWidth + pos.x],
                            tiles[next.y * newWidth + next.x]
                        );
                    }
                    robot += dir;
                }
            }
        }
        isize sum = 0;
        for (isize y = 0; y < _height; y++) {
            for (isize x = 0; x < newWidth; x++) {
                if (tiles[y * newWidth + x] == Tile::BoxLeft) {
                    sum += 100 * y + x;
                }
            }
        }
        return sum;
    }

};

int main() {
    std::optional<Warehouse> warehouse = Warehouse::parse();
    if (!warehouse) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize sum = warehouse->sum_of_gps_coordinates();
    isize sumScaled = warehouse->sum_of_gps_coordinates_scaled();
    std::println("The sum of all boxes' GPS coordinates is {}.", sum);
    std::println(
        "The sum of all boxes' GPS coordinates in the scaled-up warehouse is "
            "{}.",
        sumScaled
    );
    return EXIT_SUCCESS;
}