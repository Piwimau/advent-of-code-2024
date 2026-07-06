#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a report of the Red-Nosed reactor. */
class Report final {
private:

    /** @brief The minimum difference for a report to be considered safe. */
    static constexpr i32 MinDiff = 1;

    /** @brief The maximum difference for a report to be considered safe. */
    static constexpr i32 MaxDiff = 3;

    /** @brief The levels of the report. */
    std::vector<i32> _levels;

    /**
     * @brief Determines whether the specified levels are safe.
     *
     * The levels are considered safe if they are either all increasing or
     * all decreasing, and any two adjacent levels differ by at least `MinDiff`
     * and at most `MaxDiff`.
     *
     * @param[in] levels The levels to check.
     * @return `true` if the specified levels are safe, otherwise `false`.
     */
    static bool is_safe(std::span<const i32> levels) noexcept {
        if (std::size(levels) < 2) {
            return true;
        }
        bool increasing = false;
        bool decreasing = false;
        for (isize i = 0; i < (std::ssize(levels) - 1); i++) {
            i32 diff = levels[i + 1] - levels[i];
            if ((std::abs(diff) < MinDiff) || (std::abs(diff) > MaxDiff)) {
                return false;
            }
            if (diff < 0) {
                if (increasing) {
                    return false;
                }
                decreasing = true;
            }
            else if (diff > 0) {
                if (decreasing) {
                    return false;
                }
                increasing = true;
            }
        }
        return true;
    }

public:

    /**
     * @brief Parses a report from the standard input stream.
     *
     * The input must consist of a single line containing zero or more integers
     * separated by spaces. An example for a valid input might be the following:
     *
     * ```plaintext
     * 7 6 4 2 1
     * ```
     *
     * @return The parsed report on success, or `std::nullopt` on failure.
     */
    static std::optional<Report> parse() {
        std::string line;
        if (!std::getline(std::cin, line)) {
            return std::nullopt;
        }
        std::vector<i32> levels;
        std::istringstream iss(line);
        i32 level;
        while (iss >> level) {
            levels.push_back(level);
        }
        return Report(std::move(levels));
    }

    /**
     * @brief Initializes a new report with the specified levels.
     *
     * @param[in] levels The levels of the report.
     */
    Report(std::vector<i32> levels) : _levels(std::move(levels)) { }

    /**
     * @brief Determines whether this report is safe.
     *
     * A report is considered safe if the levels are either all increasing or
     * all decreasing, and any two adjacent levels differ by at least one and at
     * most three.
     *
     * @return `true` if this report is safe, otherwise `false`.
     */
    bool is_safe() const noexcept {
        return is_safe(_levels);
    }

    /**
     * @brief Determines whether this report is safe when allowing one removal.
     *
     * In this case, a report is considered safe if it can be made safe by
     * removing at most one level from the report.
     *
     * @return `true` if this report is safe when allowing one removal,
     * otherwise `false`.
     */
    bool is_safe_with_one_removal() const {
        if (is_safe()) {
            return true;
        }
        std::vector<i32> temp;
        temp.reserve(_levels.size() - 1);
        for (isize i = 0; i < std::ssize(_levels); i++) {
            temp.clear();
            for (isize j = 0; j < std::ssize(_levels); j++) {
                if (j != i) {
                    temp.push_back(_levels[j]);
                }
            }
            if (is_safe(temp)) {
                return true;
            }
        }
        return false;
    }

};

/**
 * @brief Parses all reports from the standard input stream.
 *
 * The input must consist of zero or more lines, each having the format as
 * described in `Report::parse()`. The individual lines must be separated with
 * a newline character.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * 7 6 4 2 1
 * 1 2 7 8 9
 * 9 7 6 2 1
 * 1 3 2 4 5
 * 8 6 4 4 1
 * 1 3 6 7 9
 * ```
 *
 * @return The parsed reports.
 */
static inline std::vector<Report> parse_reports() {
    std::vector<Report> reports;
    while (std::optional<Report> report = Report::parse()) {
        reports.push_back(std::move(*report));
    }
    return reports;
}

int main() {
    std::vector<Report> reports = parse_reports();
    if (reports.empty()) {
        std::println(
            stderr,
            "An error occurred while reading the input file."
        );
        return EXIT_FAILURE;
    }
    isize safe0 = std::ranges::count_if(
        reports,
        [](const Report& report) { return report.is_safe(); }
    );
    isize safe1 = std::ranges::count_if(
        reports,
        [](const Report& report) { return report.is_safe_with_one_removal(); }
    );
    std::println("There are {} safe reports.", safe0);
    std::println("There are {} safe reports when allowing one removal.", safe1);
    return EXIT_SUCCESS;
}