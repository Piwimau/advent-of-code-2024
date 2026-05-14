#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a two-dimensional vector. */
struct Vector {

    /** @brief The x component of the vector. */
    isize x;

    /** @brief The y component of the vector. */
    isize y;

    /**
     * @brief Determines if two specified vectors are equal.
     *
     * @param[in] lhs The left vector.
     * @param[in] rhs The right vector.
     * @return `true` if the two specified vectors are equal, otherwise `false`.
     */
    friend bool operator==(const Vector& lhs, const Vector& rhs) {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }

    /**
     * @brief Determines if two specified vectors are not equal.
     *
     * @param[in] lhs The left vector.
     * @param[in] rhs The right vector.
     * @return `true` if the two specified vectors are not equal, otherwise
     * `false`.
     */
    friend bool operator!=(const Vector& lhs, const Vector& rhs) {
        return !(lhs == rhs);
    }

};

/** @brief Represents a claw machine. */
struct Machine {

    /** @brief The movement vector for button A. */
    Vector buttonA;

    /** @brief The movement vector for button B. */
    Vector buttonB;

    /** @brief The location of the prize. */
    Vector prize;

    /**
     * @brief Parses a machine from the standard input stream.
     *
     * The input is expected to consist of three lines in the following format:
     *
     * ```plaintext
     * Button A: X+<move-x>, Y+<move-y>
     * Button B: X+<move-x>, Y+<move-y>
     * Prize: X=<x>, Y=<y>
     * ```
     *
     * Here, `<move-x>` and `<move-y>` are the x and y components of the
     * movement vectors for buttons A and B, respectively, and `<x>` and `<y>`
     * are the coordinates of the prize location. All of these values must be
     * integers.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * Button A: X+94, Y+34
     * Button B: X+22, Y+67
     * Prize: X=8400, Y=5400
     * ```
     *
     * @return The parsed machine on success, or `std::nullopt` on failure.
     */
    static std::optional<Machine> parse() {
        auto scan_line = [](
            const char* fmt,
            auto&&... args
        ) -> std::optional<std::tuple<decltype(args)...>> {
            std::string line;
            if (
                !std::getline(std::cin, line)
                    || std::sscanf(line.c_str(), fmt, &args...) != sizeof...(args)
            ) {
                return std::nullopt;
            }
            return std::tuple<decltype(args)...>(args...);
        };
        Machine m;
        if (
            !scan_line("Button A: X+%td, Y+%td", m.buttonA.x, m.buttonA.y)
                || !scan_line("Button B: X+%td, Y+%td", m.buttonB.x, m.buttonB.y)
                || !scan_line("Prize: X=%td, Y=%td", m.prize.x, m.prize.y)
        ) {
            return std::nullopt;
        }
        return m;
    }

};

/**
 * @brief Parses all machines from the standard input stream.
 *
 * The input is expected to consist of multiple machine descriptions, each in
 * the format as described in `Machine::parse()`, and separated by an empty
 * line.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * Button A: X+94, Y+34
 * Button B: X+22, Y+67
 * Prize: X=8400, Y=5400
 *
 * Button A: X+26, Y+66
 * Button B: X+67, Y+21
 * Prize: X=12748, Y=12176
 *
 * Button A: X+17, Y+86
 * Button B: X+84, Y+37
 * Prize: X=7870, Y=6450
 *
 * Button A: X+69, Y+23
 * Button B: X+27, Y+71
 * Prize: X=18641, Y=10279
 * ```
 *
 * @return The parsed machines on success, or `std::nullopt` on failure.
 */
static inline std::optional<std::vector<Machine>> parse_machines() {
    std::vector<Machine> machines;
    while (true) {
        std::optional<Machine> machine = Machine::parse();
        if (!machine) {
            break;
        }
        machines.push_back(*machine);
        std::string line;
        if (std::getline(std::cin, line)) {
            if (!line.empty()) {
                return std::nullopt;
            }
        }
        else {
            break;
        }
    }
    return machines;
}

/**
 * @brief Computes the fewest number of tokens needed to win all prizes of a
 * specified list of machines.
 *
 * @param[in] machines The list of machines for the computation.
 * @return The fewest number of tokens needed to win all prizes.
 */
static isize fewest_tokens(const std::vector<Machine>& machines) {
    isize fewestTokens = 0;
    for (const Machine& machine : machines) {
        f64 aX = static_cast<f64>(machine.buttonA.x);
        f64 aY = static_cast<f64>(machine.buttonA.y);
        f64 bX = static_cast<f64>(machine.buttonB.x);
        f64 bY = static_cast<f64>(machine.buttonB.y);
        f64 pX = static_cast<f64>(machine.prize.x);
        f64 pY = static_cast<f64>(machine.prize.y);
        f64 det = (aX * bY) - (aY * bX);
        f64 a = ((pX * bY) - (pY * bX)) / det;
        f64 b = ((aX * pY) - (aY * pX)) / det;
        // Since the button presses must be non-negative integers, we need to
        // check if our solution is actually valid before adding the tokens.
        if (
            (a >= 0.0) && (a == std::trunc(a))
                && (b >= 0.0) && (b == std::trunc(b))
        ) {
            fewestTokens += (static_cast<isize>(a) * 3) + static_cast<isize>(b);
        }
    }
    return fewestTokens;
}

int main() {
    std::optional<std::vector<Machine>> machines = parse_machines();
    if (!machines) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize fewestTokens = fewest_tokens(*machines);
    for (Machine& machine : *machines) {
        machine.prize.x += 10'000'000'000'000;
        machine.prize.y += 10'000'000'000'000;
    }
    isize fewestTokensActual = fewest_tokens(*machines);
    std::println(
        "The fewest number of tokens needed to win all prizes is {}.",
        fewestTokens
    );
    std::println(
        "The fewest number of tokens needed to win all prizes with the actual "
            "locations is {}.",
        fewestTokensActual
    );
    return EXIT_SUCCESS;
}