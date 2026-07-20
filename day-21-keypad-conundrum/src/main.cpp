#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <print>
#include <span>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a button on a keypad. */
enum class Button {
    None,
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Left,
    Up,
    Right,
    Down,
    Activate
};

template<>
struct std::hash<std::pair<Button, Button>> {
    usize operator()(const std::pair<Button, Button>& p) const noexcept {
        usize seed = 0;
        seed ^= std::hash<Button>()(p.first) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        seed ^= std::hash<Button>()(p.second) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        return seed;
    }
};

/** @brief Represents a state in the process of typing a code on a keypad. */
using State = std::tuple<Button, Button, isize>;

template<>
struct std::hash<State> {
    usize operator()(const State& state) const noexcept {
        auto [src, dst, depth] = state;
        usize seed = 0;
        seed ^= std::hash<Button>()(src) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        seed ^= std::hash<Button>()(dst) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        seed ^= std::hash<isize>()(depth) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        return seed;
    }
};

/** @brief Represents a code to be typed on a keypad. */
class Code final {
private:

    /** @brief Represents a keypad. */
    class Keypad final {
    private:

        /** @brief Represents a two-dimensional position. */
        struct Position {

            /** @brief The x-coordinate of this position. */
            isize x;

            /** @brief The y-coordinate of this position. */
            isize y;

        };

        /**
         * @brief Represents a map of the shortest sequences of button presses
         * required to move from one button to another on a keypad.
         */
        using SequenceMap = std::unordered_map<
            std::pair<Button, Button>,
            std::vector<std::vector<Button>>
        >;

        /**
         * @brief The shortest sequences of button presses required to move from
         * one button to another on this keypad.
         */
        SequenceMap _shortestSequences;

        /**
         * @brief Initializes a new keypad.
         *
         * @param[in] shortestSequences The shortest sequences of button presses
         *                              required to move from one button to
         *                              another on the keypad.
         */
        explicit Keypad(SequenceMap shortestSequences) noexcept
            : _shortestSequences(std::move(shortestSequences)) { }

        /**
         * @brief Computes the shortest sequences of button presses required to
         * move from one button to another on a keypad of the specified
         * dimensions.
         *
         * @warning The behavior is undefined if `buttons` does not contain
         * exactly `width * height` buttons, or if `width` or `height` is
         * negative. Note that the keypad is assumed to be laid out in a
         * row-major order containing only unique buttons.
         *
         * @param[in] buttons The buttons on the keypad.
         * @param[in] width   The width of the keypad.
         * @param[in] height  The height of the keypad.
         * @return The shortest sequences of button presses required to move
         * from one button to another on the keypad.
         */
        static SequenceMap shortest_sequences(
            std::span<const Button> buttons,
            isize width,
            isize height
        ) {
            assert(width >= 0);
            assert(height >= 0);
            assert(std::ssize(buttons) == width * height);
            std::unordered_map<Button, Position> positions;
            for (isize y = 0; y < height; y++) {
                for (isize x = 0; x < width; x++) {
                    Button button = buttons[y * width + x];
                    if (button != Button::None) {
                        positions[button] = { .x = x, .y = y };
                    }
                }
            }
            auto is_none = [&](Position p) -> bool {
                return buttons[p.y * width + p.x] == Button::None;
            };
            SequenceMap shortestSequences;
            for (const auto& [src, srcPos] : positions) {
                for (const auto& [dst, dstPos] : positions) {
                    std::vector<std::vector<Button>> sequences;
                    isize dx = dstPos.x - srcPos.x;
                    isize dy = dstPos.y - srcPos.y;
                    std::vector<Button> horizontalMoves(
                        std::abs(dx),
                        (dx < 0) ? Button::Left : Button::Right
                    );
                    std::vector<Button> verticalMoves(
                        std::abs(dy),
                        (dy < 0) ? Button::Up : Button::Down
                    );
                    if (!is_none({ .x = dstPos.x, .y = srcPos.y })) {
                        std::vector<Button> path = horizontalMoves;
                        path.insert_range(path.end(), verticalMoves);
                        sequences.push_back(std::move(path));
                    }
                    if (!is_none({ .x = srcPos.x, .y = dstPos.y })) {
                        std::vector<Button> path = verticalMoves;
                        path.insert_range(path.end(), horizontalMoves);
                        sequences.push_back(std::move(path));
                    }
                    if (
                        (std::ssize(sequences) == 2)
                            && (sequences[0] == sequences[1])
                    ) {
                        sequences.pop_back();
                    }
                    shortestSequences[{ src, dst }] = std::move(sequences);
                }
            }
            return shortestSequences;
        }

    public:

        /**
         * @brief Returns a numeric keypad.
         *
         * A numeric keypad is laid out as follows:
         *
         * ```plaintext
         * +---+---+---+
         * | 7 | 8 | 9 |
         * +---+---+---+
         * | 4 | 5 | 6 |
         * +---+---+---+
         * | 1 | 2 | 3 |
         * +---+---+---+
         * |   | 0 | A |
         * +---+---+---+
         * ```
         *
         * @return A numeric keypad.
         */
        static Keypad numeric() {
            std::array<Button, 12> buttons = {
                Button::Seven, Button::Eight, Button::Nine,
                Button::Four, Button::Five, Button::Six,
                Button::One, Button::Two, Button::Three,
                Button::None, Button::Zero, Button::Activate
            };
            isize width = 3;
            isize height = 4;
            return Keypad(shortest_sequences(buttons, width, height));
        }

        /**
         * @brief Returns a directional keypad.
         *
         * A directional keypad is laid out as follows:
         *
         * ```plaintext
         * +---+---+---+
         * |   | ^ | A |
         * +---+---+---+
         * | < | v | > |
         * +---+---+---+
         * ```
         *
         * @return A directional keypad.
         */
        static Keypad directional() {
            std::array<Button, 6> buttons = {
                Button::None, Button::Up, Button::Activate,
                Button::Left, Button::Down, Button::Right
            };
            isize width = 3;
            isize height = 2;
            return Keypad(shortest_sequences(buttons, width, height));
        }

        /**
         * @brief Returns the shortest sequences of button presses required to
         * move from one button to another on this keypad.
         *
         * @warning The behavior is undefined if either `src` or `dst` is not a
         * button on this keypad, or if there is no sequence of button presses
         * for moving from `src` to `dst`.
         *
         * @param[in] src The source button.
         * @param[in] dst The destination button.
         * @return The shortest sequences of button presses required to move
         * from `src` to `dst`.
         */
        const std::vector<std::vector<Button>>& shortest_sequences(
            Button src,
            Button dst
        ) const {
            auto it = _shortestSequences.find({ src, dst });
            assert(it != _shortestSequences.end());
            return it->second;
        }

    };

    /**
     * @brief Represents a cache of the shortest sequences of button presses
     * required to type a code through a sequence of directional and numeric
     * keypads.
     */
    using Cache = std::unordered_map<State, isize>;

    /** @brief The sequence of buttons to type. */
    std::vector<Button> _buttons;

    /** @brief The numeric part of the code. */
    isize _numericPart;

    /**
     * @brief Initializes a new code with the specified sequence of buttons and
     * numeric part.
     *
     * @warning The behavior is undefined if `buttons` contains buttons other
     * than digits or the activation button, or if `numericPart` is negative.
     *
     * @param[in] buttons     The sequence of buttons to type.
     * @param[in] numericPart The numeric part of the code.
     */
    Code(std::vector<Button> buttons, isize numericPart) noexcept
        : _buttons(std::move(buttons)),
          _numericPart(numericPart) {
        assert(_numericPart >= 0);
    }

    /**
     * @brief Returns the length of the shortest sequence of button presses
     * required to type a code through a sequence of directional and numeric
     * keypads, starting from a specified source button and ending at a
     * destination button, with a specified number of keypads remaining to be
     * used.
     *
     * @param[in]      state  The current state of the typing process.
     * @param[in]      keypad The current keypad being used.
     * @param[in, out] cache  A cache of previously computed shortest sequence
     *                        lengths.
     * @return The length of the shortest sequence of button presses required to
     * type the code through the remaining keypads.
     */
    static isize shortest_len(
        const State& state,
        const Keypad& keypad,
        Cache& cache
    ) {
        auto [src, dst, depth] = state;
        if (depth == 0) {
            return std::ssize(keypad.shortest_sequences(src, dst).front()) + 1;
        }
        auto it = cache.find(state);
        if (it != cache.end()) {
            return it->second;
        }
        isize shortestLen = std::numeric_limits<isize>::max();
        for (
            const std::vector<Button>& sequence
                : keypad.shortest_sequences(src, dst)
        ) {
            isize len = 0;
            Button current = Button::Activate;
            for (Button button : sequence) {
                len += shortest_len(
                    { current, button, depth - 1 },
                    keypad,
                    cache
                );
                current = button;
            }
            len += shortest_len(
                { current, Button::Activate, depth - 1 },
                keypad,
                cache
            );
            shortestLen = std::min(shortestLen, len);
        }
        cache[state] = shortestLen;
        return shortestLen;
    }

    /**
     * @brief Returns the length of the shortest sequence of button presses
     * required to type this code through a sequence of directional and numeric
     * keypads.
     *
     * @warning The behavior is undefined if `directionalKeypads` is less than
     * `1`.
     *
     * @param[in] directionalKeypads The number of directional keypads to be
     *                               used.
     * @return The length of the shortest sequence of button presses required to
     * type this code.
     */
    isize shortest_len(isize directionalKeypads) const {
        assert(directionalKeypads >= 1);
        Keypad numeric = Keypad::numeric();
        Keypad directional = Keypad::directional();
        Cache cache;
        isize totalLen = 0;
        Button current = Button::Activate;
        for (Button button : _buttons) {
            isize shortestLen = std::numeric_limits<isize>::max();
            for (
                const std::vector<Button>& sequence
                    : numeric.shortest_sequences(current, button)
            ) {
                isize len = 0;
                Button innerCurrent = Button::Activate;
                for (Button innerButton : sequence) {
                    len += shortest_len(
                        { innerCurrent, innerButton, directionalKeypads - 1 },
                        directional,
                        cache
                    );
                    innerCurrent = innerButton;
                }
                len += shortest_len(
                    { innerCurrent, Button::Activate, directionalKeypads - 1 },
                    directional,
                    cache
                );
                shortestLen = std::min(shortestLen, len);
            }
            totalLen += shortestLen;
            current = button;
        }
        return totalLen;
    }

public:

    /**
     * @brief Parses a code from a specified line of text.
     *
     * The line of text must contain zero or more digits (indicating button
     * presses on a numeric keypad), followed by a single uppercase `A`
     * (indicating a press of the activation button).
     *
     * Examples for valid lines of text might be the following:
     *
     * ```plaintext
     * 029A
     * 980A
     * 179A
     * 456A
     * 379A
     * ```
     *
     * @param[in] line The line of text to parse.
     * @return The parsed code on success, or `std::nullopt` on failure.
     */
    static std::optional<Code> parse(std::string_view line) {
        if (line.empty() || (line.back() != 'A')) {
            return std::nullopt;
        }
        std::vector<Button> buttons;
        isize numericPart = 0;
        // We strip the final character, which is known to be the activation
        // button at this point. We add it separately after parsing the numeric
        // part.
        line.remove_suffix(1);
        for (char c : line) {
            switch (c) {
                case '0':
                    buttons.push_back(Button::Zero);
                    break;
                case '1':
                    buttons.push_back(Button::One);
                    break;
                case '2':
                    buttons.push_back(Button::Two);
                    break;
                case '3':
                    buttons.push_back(Button::Three);
                    break;
                case '4':
                    buttons.push_back(Button::Four);
                    break;
                case '5':
                    buttons.push_back(Button::Five);
                    break;
                case '6':
                    buttons.push_back(Button::Six);
                    break;
                case '7':
                    buttons.push_back(Button::Seven);
                    break;
                case '8':
                    buttons.push_back(Button::Eight);
                    break;
                case '9':
                    buttons.push_back(Button::Nine);
                    break;
                default:
                    return std::nullopt;
            }
            numericPart = numericPart * 10 + (c - '0');
        }
        buttons.push_back(Button::Activate);
        return Code(std::move(buttons), numericPart);
    }

    /**
     * @brief Returns the complexity of this code.
     *
     * The complexity of a code is defined as the shortest sequence of button
     * presses required to type the code through a sequence of directional and
     * numeric keypads, multiplied by the numeric part of the code.
     *
     * @warning The behavior is undefined if `directionalKeypads` is less than
     * `1`.
     *
     * @param[in] directionalKeypads The number of directional keypads to be
     *                               used, `2` by default.
     * @return The complexity of this code.
     */
    isize complexity(isize directionalKeypads = 2) const {
        return shortest_len(directionalKeypads) * _numericPart;
    }

};

/**
 * @brief Parses a sequence of codes from standard input stream.
 *
 * The input must consist of zero or more lines, each containing a code with
 * the format as described in `Code::parse()`. The individual codes must be
 * separated by newlines.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * 029A
 * 980A
 * 179A
 * 456A
 * 379A
 * ```
 *
 * @return The parsed sequence of codes on success, or `std::nullopt` on
 * failure.
 */
static std::optional<std::vector<Code>> parse_codes() {
    std::vector<Code> codes;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::optional<Code> code = Code::parse(line);
        if (!code) {
            return std::nullopt;
        }
        codes.push_back(std::move(*code));
    }
    return codes;
}

int main() {
    std::optional<std::vector<Code>> codes = parse_codes();
    if (!codes) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize sumOfComplexities2 = std::transform_reduce(
        codes->begin(),
        codes->end(),
        static_cast<isize>(0),
        std::plus<isize>(),
        [](const Code& code) { return code.complexity(2); }
    );
    isize sumOfComplexities25 = std::transform_reduce(
        codes->begin(),
        codes->end(),
        static_cast<isize>(0),
        std::plus<isize>(),
        [](const Code& code) { return code.complexity(25); }
    );
    std::println(
        "The sum of the complexities with 2 directional keypads is {}.",
        sumOfComplexities2
    );
    std::println(
        "The sum of the complexities with 25 directional keypads is {}.",
        sumOfComplexities25
    );
    return EXIT_SUCCESS;
}