#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <unordered_set>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a two-dimensional vector. */
struct Vector {

    /** @brief The x-component of the vector. */
    isize x;

    /** @brief The y-component of the vector. */
    isize y;

    /** @brief Initializes a new vector with default components. */
    Vector() : x(0), y(0) { }

    /**
     * @brief Initializes a new vector with the specified components.
     *
     * @param[in] x The x-component of the vector.
     * @param[in] y The y-component of the vector.
     */
    Vector(isize x, isize y) : x(x), y(y) { }

    /**
     * @brief Determines whether this vector is equal to a specified vector.
     *
     * The equality is determined component-wise, i.e., two vectors are equal
     * if and only if their x- and y-components are equal.
     *
     * @param[in] other The vector to compare this vector with.
     * @return `true` if this vector is equal to the specified vector, otherwise
     * `false`.
     */
    bool operator==(const Vector& other) const {
        return (x == other.x) && (y == other.y);
    }

    /**
     * @brief Determines whether this vector is not equal to a specified vector.
     *
     * The inequality is determined component-wise, i.e., two vectors are not
     * equal if and only if at least one of their components differ.
     *
     * @param[in] other The vector to compare this vector with.
     * @return `true` if this vector is not equal to the specified vector,
     * otherwise `false`.
     */
    bool operator!=(const Vector& other) const {
        return !(*this == other);
    }

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
    Vector& operator+=(const Vector& other) {
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
    Vector& operator-=(const Vector& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * @brief Multiplies this vector by a specified scalar.
     *
     * The multiplication is performed component-wise, i.e., the x- and
     * y-components of this vector are multiplied by the specified scalar.
     *
     * @param[in] scalar The scalar to multiply this vector by.
     * @return A reference to this vector.
     */
    Vector& operator*=(isize scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /**
     * @brief Adds two specified vectors together.
     *
     * The addition is performed component-wise, i.e., the x- and y-components
     * of the resulting vector are the sums of the x- and y-components of the
     * specified vectors, respectively.
     *
     * @param[in] lhs The left vector.
     * @param[in] rhs The right vector.
     * @return The vector resulting from adding the specified vectors together.
     */
    friend Vector operator+(Vector lhs, const Vector& rhs) {
        return lhs += rhs;
    }

    /**
     * @brief Subtracts one specified vector from another specified vector.
     *
     * The subtraction is performed component-wise, i.e., the x- and
     * y-components of the resulting vector are the differences of the x- and
     * y-components of the specified vectors, respectively.
     *
     * @param[in] lhs The left vector.
     * @param[in] rhs The right vector.
     * @return The vector resulting from subtracting the right vector from the
     * left vector.
     */
    friend Vector operator-(Vector lhs, const Vector& rhs) {
        return lhs -= rhs;
    }

    /**
     * @brief Multiplies a specified vector by a specified scalar.
     *
     * The multiplication is performed component-wise, i.e., the x- and
     * y-components of the resulting vector are the products of the x- and
     * y-components of the specified vector and the specified scalar,
     * respectively.
     *
     * @param[in] vector The vector to multiply.
     * @param[in] scalar The scalar to multiply the vector by.
     * @return The vector resulting from multiplying the specified vector by the
     * specified scalar.
     */
    friend Vector operator*(Vector vector, isize scalar) {
        return vector *= scalar;
    }

};

template<>
struct std::hash<Vector> {
    usize operator()(const Vector& vector) const {
        usize h1 = std::hash<isize>()(vector.x);
        usize h2 = std::hash<isize>()(vector.y);
        return h1 ^ ((h2 << 32) | (h2 >> 32));
    }
};

/** @brief Represents an antenna on the grid. */
struct Antenna {

    /** @brief The position of the antenna. */
    Vector position;

    /** @brief The frequency of the antenna. */
    char frequency;

    /**
     * @brief Initializes a new antenna with the specified position and
     * frequency.
     *
     * @param[in] position  The position of the antenna.
     * @param[in] frequency The frequency of the antenna.
     */
    Antenna(Vector position, char frequency)
        : position(position), frequency(frequency) { }

};

/** @brief Represents a two-dimensional grid of antennas. */
class Grid final {
private:

    /** @brief The antennas on the grid. */
    std::vector<Antenna> antennas;

    /** @brief The width of the grid. */
    isize width;

    /** @brief The height of the grid. */
    isize height;

    /**
     * @brief Initializes a new grid with the specified antennas, width, and
     * height.
     *
     * @param[in] antennas The antennas on the grid.
     * @param[in] width    The width of the grid.
     * @param[in] height   The height of the grid.
     */
    Grid(std::vector<Antenna> antennas, isize width, isize height)
        : antennas(std::move(antennas)), width(width), height(height) {
        assert(this->width >= 0);
        assert(this->height >= 0);
    }

    /**
     * @brief Determines whether a specified position exists within the bounds
     * of this grid.
     *
     * @param[in] position The position to check.
     * @return `true` if the specified position exists within the bounds of this
     * grid, otherwise `false`.
     */
    bool exists(Vector position) const {
        return (position.x >= 0) && (position.x < width)
            && (position.y >= 0) && (position.y < height);
    }

public:

    /**
     * @brief Parses a grid from the standard input stream.
     *
     * The input is expected to consist of zero or more lines of text, each
     * containing the same number of characters. Characters other than `'.'`
     * represent antennas with the corresponding frequency. The individual lines
     * must be separated with a newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * ............
     * ........0...
     * .....0......
     * .......0....
     * ....0.......
     * ......A.....
     * ............
     * ............
     * ........A...
     * .........A..
     * ............
     * ............
     * ```
     *
     * @return The parsed grid on success, or `std::nullopt` on failure.
     */
    static std::optional<Grid> parse() {
        std::vector<Antenna> antennas;
        isize width = 0;
        isize height = 0;
        bool foundWidth = false;
        std::string line;
        while (std::getline(std::cin, line )) {
            if (!foundWidth) {
                width = std::ssize(line);
                foundWidth = true;
            }
            else if (std::ssize(line) != width) {
                return std::nullopt;
            }
            for (isize x = 0; x < width; x++) {
                char c = line[x];
                if (c != '.') {
                    antennas.emplace_back(Vector(x, height), c);
                }
            }
            height++;
        }
        return Grid(std::move(antennas), width, height);
    }

    /**
     * @brief Returns the number of unique locations within the bounds of the
     * grid that contain an antinode.
     *
     * @return The number of unique locations within the bounds of the grid that
     * contain an antinode.
     */
    isize antinodes() const {
        std::unordered_set<Vector> antinodes;
        for (const auto& [l, r] : std::views::cartesian_product(antennas, antennas)) {
            if ((l.position != r.position) && (l.frequency == r.frequency)) {
                Vector leftAntinode = (l.position * 2) - r.position;
                if (exists(leftAntinode)) {
                    antinodes.insert(leftAntinode);
                }
                Vector rightAntinode = (r.position * 2) - l.position;
                if (exists(rightAntinode)) {
                    antinodes.insert(rightAntinode);
                }
            }
        }
        return std::ssize(antinodes);
    }

    /**
     * @brief Returns the number of unique locations within the bounds of the
     * grid that contain an antinode using the updated model.
     *
     * @return The number of unique locations within the bounds of the grid that
     * contain an antinode using the updated model.
     */
    isize antinodes_updated() const {
        std::unordered_set<Vector> antinodes;
        for (const auto& [l, r] : std::views::cartesian_product(antennas, antennas)) {
            if ((l.position != r.position) && (l.frequency == r.frequency)) {
                for (isize k = 0; ; k++) {
                    Vector leftAntinode = (l.position * (k + 1))
                        - (r.position * k);
                    if (!exists(leftAntinode)) {
                        break;
                    }
                    antinodes.insert(leftAntinode);
                }
                for (isize k = 0; ; k++) {
                    Vector rightAntinode = (r.position * (k + 1))
                        - (l.position * k);
                    if (!exists(rightAntinode)) {
                        break;
                    }
                    antinodes.insert(rightAntinode);
                }
            }
        }
        return std::ssize(antinodes);
    }

};

int main() {
    std::optional<Grid> grid = Grid::parse();
    if (!grid) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize antinodes = grid->antinodes();
    isize antinodesUpdated = grid->antinodes_updated();
    std::println("{} unique locations contain an antinode.", antinodes);
    std::println(
        "{} unique locations contain an antinode using the updated model.",
        antinodesUpdated
    );
    return EXIT_SUCCESS;
}