#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

struct Rule;
using Rules = std::unordered_map<i32, std::vector<Rule>>;

class Update;
using Updates = std::vector<Update>;

/** @brief Represents a rule for page ordering in the print queue. */
struct Rule {

    /** @brief The page number that must be printed before the other page. */
    i32 before;

    /** @brief The page number that must be printed after the other page. */
    i32 after;

    /**
     * @brief Parses a rule from a specified line of text.
     *
     * The line of text must have the following format:
     *
     * ```plaintext
     * <before>|<after>
     * ```
     *
     * Here, `<before>` and `<after>` are positive integers representing the
     * page numbers that must be printed before and after each other,
     * respectively. There must be exactly one pipe character (`|`) separating
     * the two page numbers, and there must be no extra whitespace before,
     * after, or between the page numbers and the pipe character.
     *
     * An example for a valid line of text may be the following:
     *
     * ```plaintext
     * 47|53
     * ```
     *
     * @param[in] line The line of text to parse.
     * @return The parsed rule on success, or `std::nullopt` on failure.
     */
    static std::optional<Rule> parse(const std::string& line) {
        std::istringstream iss(line);
        i32 before;
        i32 after;
        char separator;
        if (
            (iss >> before >> separator >> after) && (before >= 0)
                && (after >= 0) && (separator == '|') && iss.eof()
        ) {
            return Rule(before, after);
        }
        return std::nullopt;
    }

};

/** @brief Represents an update to be printed in the print queue. */
class Update final {
private:

    /** @brief The page numbers to be printed. */
    std::vector<i32> pages;

    /**
     * @brief Initializes a new update with the specified page numbers.
     *
     * @param[in] pages The page numbers to be printed.
     */
    Update(std::vector<i32> pages) : pages(std::move(pages)) { }

public:

    /**
     * @brief Parses an update from a specified line of text.
     *
     * The line of text must have the following format:
     *
     * ```plaintext
     * <page1>,<page2>,...,<pageN>
     * ```
     *
     * Here, `<page1>`, `<page2>`, ..., `<pageN>` are positive integers
     * representing the page numbers to be printed in the update. The page
     * numbers (if any) must be separated by commas, and there must be no extra
     * whitespace before, after, or between the page numbers.
     *
     * @param[in] line The line of text to parse.
     * @return The parsed update on success, or `std::nullopt` on failure.
     */
    static std::optional<Update> parse(const std::string& line) {
        std::istringstream iss(line);
        std::vector<i32> pages;
        i32 page;
        char separator;
        while (iss >> page) {
            if (page < 0) {
                return std::nullopt;
            }
            pages.push_back(page);
            if (!(iss >> separator)) {
                break;
            }
            if (separator != ',') {
                return std::nullopt;
            }
        }
        if (iss.eof()) {
            return Update(std::move(pages));
        }
        return std::nullopt;
    }

    /**
     * @brief Determines whether this update is correctly ordered according to
     * the specified rules.
     *
     * @param[in] rules The rules for page ordering in the print queue.
     * @return `true` if this update is correctly ordered according to the
     * specified rules, otherwise `false`.
     */
    bool is_ordered(const Rules& rules) const {
        for (auto page = pages.begin(); page != pages.end(); page++) {
            const auto it = rules.find(*page);
            if (it == rules.end()) {
                continue;
            }
            const auto& [_, relevantRules] = *it;
            for (const Rule& rule : relevantRules) {
                if (rule.before == *page) {
                    auto afterIt = std::ranges::find(pages, rule.after);
                    if ((afterIt != pages.end()) && (afterIt < page)) {
                        return false;
                    }
                }
                else {
                    auto beforeIt = std::ranges::find(pages, rule.before);
                    if ((beforeIt != pages.end()) && (beforeIt > page)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Reorders the page numbers in this update according to the
     * specified rules.
     *
     * @param[in] rules The rules for page ordering in the print queue.
     */
    void order(const Rules& rules) {
        std::ranges::sort(
            pages,
            [&rules](i32 l, i32 r) {
                const auto it = rules.find(l);
                if (it == rules.end()) {
                    return false;
                }
                const auto& [_, relevantRules] = *it;
                for (const Rule& rule : relevantRules) {
                    if ((rule.before == l) && (rule.after == r)) {
                        return true;
                    }
                }
                return false;
            }
        );
    }

    /**
     * @brief Returns the middle page number of this update.
     *
     * @warning The behavior is undefined if this update does not contain an odd
     * number of page numbers.
     *
     * @return The middle page number of this update.
     */
    i32 middle_page() const noexcept {
        assert((pages.size() % 2) == 1);
        return pages[pages.size() / 2];
    }

};

/**
 * @brief Parses the rules and updates from the standard input stream.
 *
 * The input is expected to consist of one or more lines of text, which are
 * separated into two sections by a blank line. The first section contains the
 * rules for page ordering in the print queue, and the second section contains
 * the updates to be printed in the print queue. See the documentation of
 * `Rule::parse()` and `Update::parse()` for more details on the expected format
 * of the lines of text in each section.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * 47|53
 * 97|13
 * 97|61
 * 97|47
 * 75|29
 * 61|13
 * 75|53
 * 29|13
 * 97|29
 * 53|29
 * 61|53
 * 97|53
 * 61|29
 * 47|13
 * 75|47
 * 97|75
 * 47|61
 * 75|61
 * 47|29
 * 75|13
 * 53|13
 *
 * 75,47,61,53,29
 * 97,61,53,29,13
 * 75,29,13
 * 75,97,47,61,53
 * 61,13,29
 * 97,13,75,29,47
 * ```
 *
 * @return A pair containing the parsed rules and updates on success, or
 * `std::nullopt` on failure.
 */
static inline std::optional<std::pair<Rules, Updates>> parse_input() {
    Rules rules;
    Updates updates;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        std::optional<Rule> rule = Rule::parse(line);
        if (!rule) {
            return std::nullopt;
        }
        rules[rule->before].push_back(*rule);
        rules[rule->after].push_back(*rule);
    }
    while (std::getline(std::cin, line)) {
        std::optional<Update> update = Update::parse(line);
        if (!update) {
            return std::nullopt;
        }
        updates.push_back(*update);
    }
    return std::make_pair(std::move(rules), std::move(updates));
}

/**
 * @brief Computes the sum of the middle page numbers of the correctly-ordered
 * updates.
 *
 * @param[in] rules   The rules for page ordering in the print queue.
 * @param[in] updates The updates to be printed in the print queue.
 * @return The sum of the middle page numbers of the correctly-ordered updates.
 */
static inline i32 sum_of_middle_pages_ordered(
    const Rules& rules,
    const Updates& updates
) {
    i32 sum = 0;
    for (const Update& update : updates) {
        if (update.is_ordered(rules)) {
            sum += update.middle_page();
        }
    }
    return sum;
}

/**
 * @brief Computes the sum of the middle page numbers of the unordered updates
 * after reordering them according to the rules.
 *
 * @param[in] rules   The rules for page ordering in the print queue.
 * @param[in] updates The updates to be printed in the print queue.
 * @return The sum of the middle page numbers of the unordered updates after
 * reordering them according to the rules.
 */
static inline i32 sum_of_middle_pages_unordered(
    const Rules& rules,
    const Updates& updates
) {
    i32 sum = 0;
    for (const Update& update : updates) {
        if (!update.is_ordered(rules)) {
            Update temp = update;
            temp.order(rules);
            sum += temp.middle_page();
        }
    }
    return sum;
}

int main() {
    std::optional<std::pair<Rules, Updates>> input = parse_input();
    if (!input) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    auto& [rules, updates] = *input;
    i32 sumOrdered = sum_of_middle_pages_ordered(rules, updates);
    i32 sumUnordered = sum_of_middle_pages_unordered(rules, updates);
    std::println(
        "The sum of the middle page numbers of the correctly-ordered updates "
            "is {}.",
        sumOrdered
    );
    std::println(
        "The sum of the middle page numbers of the unordered updates after "
            "reordering them according to the rules is {}.",
        sumUnordered
    );
    return EXIT_SUCCESS;
}