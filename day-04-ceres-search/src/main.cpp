#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a two-dimensional vector. */
struct Vector {

    /** @brief The x-component of the vector. */
    isize x;

    /** @brief The y-component of the vector. */
    isize y;

    /**
     * @brief Initializes a new vector with the specified components.
     *
     * @param[in] x The x-component of the vector, zero by default.
     * @param[in] y The y-component of the vector, zero by default.
     */
    constexpr Vector(isize x, isize y) : x(x), y(y) { }

    /**
     * @brief Adds a specified vector to this vector.
     *
     * The addition is performed component-wise, i.e., the x- and y-components
     * of the specified vector are added to the x- and y-components of this
     * vector, respectively.
     *
     * @param[in] other The vector to add to this vector.
     * @return A reference to this vector.
     */
    constexpr Vector& operator+=(const Vector& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    /**
     * @brief Subtracts a specified vector from this vector.
     *
     * The subtraction is performed component-wise, i.e., the x- and
     * y-components of the specified vector are subtracted from the x- and
     * y-components of this vector, respectively.
     *
     * @param[in] other The vector to subtract from this vector.
     * @return A reference to this vector.
     */
    constexpr Vector& operator-=(const Vector& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * @brief Adds two specified vectors.
     *
     * The addition is performed component-wise, i.e., the x- and y-components
     * of the second vector are added to the x- and y-components of the first
     * vector, respectively.
     *
     * @param[in] left  The first vector.
     * @param[in] right The second vector.
     * @return A vector that is the result of adding the two specified vectors.
     */
    friend constexpr Vector operator+(
        Vector left,
        const Vector& right
    ) noexcept {
        left += right;
        return left;
    }

    /**
     * @brief Subtracts two specified vectors.
     *
     * The subtraction is performed component-wise, i.e., the x- and
     * y-components of the second vector are subtracted from the x- and
     * y-components of the first vector, respectively.
     *
     * @param[in] left  The first vector.
     * @param[in] right The second vector.
     * @return A vector that is the result of subtracting the second vector from
     * the first vector.
     */
    friend constexpr Vector operator-(
        Vector left,
        const Vector& right
    ) noexcept {
        left -= right;
        return left;
    }

};

/** @brief Represents a two-dimensional grid for a word search puzzle. */
class Grid final {
private:

    /** @brief The possible directions for searching words in the grid. */
    static constexpr std::array<Vector, 8> Offsets = {
        Vector(-1, -1),
        Vector(0, -1),
        Vector(1, -1),
        Vector(-1, 0),
        Vector(1, 0),
        Vector(-1, 1),
        Vector(0, 1),
        Vector(1, 1)
    };

    /** 
     * @brief The corner characters for X-shaped occurrences of the word "MAS".
     */
    static constexpr std::array<std::array<char, 4>, 4> XmasCorners = {
        std::array<char, 4>{'M', 'S', 'M', 'S'},
        std::array<char, 4>{'M', 'M', 'S', 'S'},
        std::array<char, 4>{'S', 'S', 'M', 'M'},
        std::array<char, 4>{'S', 'M', 'S', 'M'}
    };

    /**
     * @brief The tiles of the grid.
     *
     * @note This is actually a two-dimensional array of `width * height`
     * characters stored as a one-dimensional array one in row-major order.
     */
    std::vector<char> _tiles;

    /** @brief The width of the grid. */
    isize _width;

    /** @brief The height of the grid. */
    isize _height;

    /**
     * @brief Initializes a new grid with the specified tiles, width, and
     * height.
     *
     * @warning The behavior is undefined if `tiles` does not contain exactly
     * `width * height` characters, or if `width` or `height` is negative.
     *
     * @param[in] tiles  The tiles of the grid.
     * @param[in] width  The width of the grid.
     * @param[in] height The height of the grid.
     */
    Grid(std::vector<char> tiles, isize width, isize height)
        : _tiles(std::move(tiles)), _width(width), _height(height) {
        assert(_width >= 0);
        assert(_height >= 0);
        assert(std::ssize(_tiles) == (_width * _height));
    }

    /**
     * @brief Determines whether a specified position exists in this grid.
     *
     * @return `true` if the specified position exists in this grid, otherwise
     * `false`.
     */
    bool exists(Vector position) const noexcept {
        return (position.x >= 0) && (position.x < _width)
            && (position.y >= 0) && (position.y < _height);
    }

    /**
     * @brief Returns the tile at a specified position in this grid.
     *
     * @warning The behavior is undefined if the specified position does not
     * exist in this grid.
     *
     * @param[in] position The position of the tile.
     * @return The tile at the specified position in this grid.
     */
    char at(Vector position) const noexcept {
        assert(exists(position));
        return _tiles[position.y * _width + position.x];
    }

public:

    /**
     * @brief Parses a grid from the standard input stream.
     *
     * The input is expected to consist of zero or more lines of text, each
     * containing the same number of characters. The individual lines must be
     * separated with a newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * MMMSXXMASM
     * MSAMXMSMSA
     * AMXSXMAAMM
     * MSAMASMSMX
     * XMASAMXAMM
     * XXAMMXXAMA
     * SMSMSASXSS
     * SAXAMASAAA
     * MAMMMXMMMM
     * MXMXAXMASX
     * ```
     *
     * @return The parsed grid on success, or `std::nullopt` on failure.
     */
    static std::optional<Grid> parse() {
        std::vector<char> tiles;
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
            tiles.append_range(line);
            height++;
        }
        return Grid(std::move(tiles), width, height);
    }

    /**
     * @brief Counts the number of occurrences of the word "XMAS" in this grid.
     *
     * The word can be found in the grid in any of the eight possible
     * directions, i.e., horizontally, vertically, or diagonally, and in both
     * forward and backward directions.
     *
     * @return The number of occurrences of the word "XMAS" in this grid.
     */
    isize count_xmas() const noexcept {
        isize count = 0;
        for (isize y = 0; y < _height; y++) {
            for (isize x = 0; x < _width; x++) {
                for (Vector offset : Offsets) {
                    Vector pos(x, y);
                    bool found = true;
                    for (const char c : { 'X', 'M', 'A', 'S' }) {
                        if (!exists(pos) || (at(pos) != c)) {
                            found = false;
                            break;
                        }
                        pos += offset;
                    }
                    if (found) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    /**
     * @brief Counts the number of X-shaped occurrences of the word "MAS" in
     * this grid.
     *
     * An X-shaped occurrence consists of two "MAS" words arranged in the shape
     * of an "X". Each "MAS" can be written forwards or backwards.
     *
     * @return The number of X-shaped occurrences of the word "MAS" in this
     * grid.
     */
    isize count_x_mas() const noexcept {
        isize count = 0;
        for (isize y = 0; y < _height; y++) {
            for (isize x = 0; x < _width; x++) {
                Vector pos(x, y);
                if (at(pos) != 'A') {
                    continue;
                }
                Vector topLeft = pos + Vector(-1, -1);
                Vector topRight = pos + Vector(1, -1);
                Vector bottomLeft = pos + Vector(-1, 1);
                Vector bottomRight = pos + Vector(1, 1);
                if (
                    !exists(topLeft) || !exists(topRight)
                        || !exists(bottomLeft) || !exists(bottomRight)
                ) {
                    continue;
                }
                std::array<char, 4> corners = {
                    at(topLeft),
                    at(topRight),
                    at(bottomLeft),
                    at(bottomRight)
                };
                for (const auto& xMasCorners : XmasCorners) {
                    if (corners == xMasCorners) {
                        count++;
                        break;
                    }
                }
            }
        }
        return count;
    }

};

int main() {
    std::optional<Grid> grid = Grid::parse();
    if (!grid) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize countXmas = grid->count_xmas();
    isize countXMas = grid->count_x_mas();
    std::println("The word \"XMAS\" appears {} times.", countXmas);
    std::println("\"X-MAS\" appears {} times.", countXMas);
    return EXIT_SUCCESS;
}