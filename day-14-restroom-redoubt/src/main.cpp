#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <print>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a robot moving on a map of tiles. */
struct Robot {

    /** @brief The x-coordinate of the robot. */
    isize x;

    /** @brief The y-coordinate of the robot. */
    isize y;

    /** @brief The velocity of the robot on the x-axis. */
    isize dx;

    /** @brief The velocity of the robot on the y-axis. */
    isize dy;

    /**
     * @brief Parses a robot from the standard input stream.
     *
     * The input is expected to consist of a single line of text in the
     * following format:
     *
     * ```plaintext
     * p=<x>,<y> v=<x>,<y>
     * ```
     *
     * Here, `<x>` and `<y>` are integers that represent the position and
     * velocity of the robot on the x- and y-axis, respectively.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * p=0,4 v=3,-3
     * ```
     *
     * @return The parsed robot on success, otherwise `std::nullopt`.
     */
    static std::optional<Robot> parse() {
        Robot robot;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return std::nullopt;
        }
        if (std::sscanf(
                line.c_str(),
                "p=%td,%td v=%td,%td",
                &robot.x,
                &robot.y,
                &robot.dx,
                &robot.dy
            )
            != 4) {
            return std::nullopt;
        }
        return robot;
    }

};

/** @brief Represents a map of robots. */
class Map final {
private:

    /** @brief The width of the map. */
    static constexpr isize WIDTH = 101;

    /** @brief The height of the map. */
    static constexpr isize HEIGHT = 103;

    /** @brief The x-coordinate of the center column. */
    static constexpr isize CENTER_X = WIDTH / 2;

    /** @brief The y-coordinate of the center row. */
    static constexpr isize CENTER_Y = HEIGHT / 2;

    /** @brief The robots moving on the map. */
    std::vector<Robot> robots;

    /**
     * @brief Initializes a new map with a specified list of robots.
     *
     * @param[in] robots The robots moving on the map.
     */
    Map(std::vector<Robot> robots) : robots(std::move(robots)) { }

public:

    /**
     * @brief Parses a map from the standard input stream.
     *
     * The input is expected to consist of zero or more lines of text in the
     * format described in `Robot::parse()`. The individual lines must be
     * separated with a newline character.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * p=0,4 v=3,-3
     * p=6,3 v=-1,-3
     * p=10,3 v=-1,2
     * p=2,0 v=2,-1
     * p=0,0 v=1,3
     * p=3,0 v=-2,-2
     * p=7,6 v=-1,-3
     * p=3,0 v=-1,-2
     * p=9,3 v=2,3
     * p=7,3 v=-1,2
     * p=2,4 v=2,-3
     * p=9,5 v=-3,-3
     * ```
     *
     * @return The parsed map on success, or `std::nullopt` on failure.
     */
    static std::optional<Map> parse() {
        std::vector<Robot> robots;
        while (std::optional<Robot> robot = Robot::parse()) {
            robots.push_back(*robot);
        }
        if (!std::cin.eof()) {
            return std::nullopt;
        }
        return Map(std::move(robots));
    }

    /**
     * @brief Returns the safety factor of this map after a specified number of
     * seconds.
     *
     * The safety factor is defined as the product of the number of robots in
     * each quadrant of the map after the specified number of seconds. Robots
     * that are exactly in the middle (horizontally or vertically) do not count
     * towards the result.
     *
     * @param[in] seconds The number of seconds to check.
     * @return The safety factor of this map after the specified number of
     * seconds.
     */
    isize safety_factor(isize seconds) const noexcept {
        assert(seconds >= 0);
        isize topLeft = 0;
        isize bottomLeft = 0;
        isize bottomRight = 0;
        isize topRight = 0;
        for (const Robot& robot : robots) {
            isize x = (robot.x + (robot.dx * seconds % WIDTH) + WIDTH) % WIDTH;
            isize y = (robot.y + (robot.dy * seconds % HEIGHT) + HEIGHT) % HEIGHT;
            if (x < CENTER_X) {
                if (y < CENTER_Y) {
                    topLeft++;
                }
                else if (y > CENTER_Y) {
                    bottomLeft++;
                }
            }
            else if (x > CENTER_X) {
                if (y > CENTER_Y) {
                    bottomRight++;
                }
                else if (y < CENTER_Y) {
                    topRight++;
                }
            }
        }
        return topLeft * bottomLeft * bottomRight * topRight;
    }

    /**
     * @brief Returns the safety factor of this map.
     *
     * The safety factor is defined as the product of the number of robots in
     * each quadrant of the map after 100 seconds. Robots that are exactly in
     * the middle (horizontally or vertically) do not count towards the result.
     *
     * @return The safety factor of this map.
     */
    isize safety_factor() const noexcept {
        return safety_factor(100);
    }

    /**
     * @brief Returns the fewest number of seconds that must elapse for the
     * robots on this map to form a Christmas tree.
     *
     * @return The fewest number of seconds to reach the easter egg.
     */
    isize fewest_seconds_to_easter_egg() const {
        isize minSafetyFactor = std::numeric_limits<isize>::max();
        isize fewestSeconds = 0;
        for (isize i = 0; i < WIDTH * HEIGHT; i++) {
            isize seconds = i + 1;
            isize safetyFactor = safety_factor(seconds);
            if (safetyFactor < minSafetyFactor) {
                minSafetyFactor = safetyFactor;
                fewestSeconds = seconds;
            }
        }
        return fewestSeconds;
    }

};

int main() {
    std::optional<Map> map = Map::parse();
    if (!map) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize safetyFactor = map->safety_factor();
    isize fewestSeconds = map->fewest_seconds_to_easter_egg();
    std::println("The safety factor of the map is {}.", safetyFactor);
    std::println(
        "The fewest number of seconds to the easter egg is {}.",
        fewestSeconds
    );
    return EXIT_SUCCESS;
}