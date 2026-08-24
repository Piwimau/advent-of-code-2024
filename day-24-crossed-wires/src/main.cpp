#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a variable in a circuit. */
class Variable final {
public:

    /** @brief The length of the input numbers (in bits). */
    static constexpr isize INPUT_LENGTH = 44;

private:

    /** @brief The length of a variable's name. */
    static constexpr isize NAME_LENGTH = 3;

    /** @brief The name of this variable. */
    std::array<char, NAME_LENGTH> _name;

    /**
     * @brief Initializes a new variable with a specified name.
     *
     * @param[in] name The name of the variable.
     */
    constexpr explicit Variable(std::array<char, NAME_LENGTH> name) noexcept
        : _name(std::move(name)) { }

public:

    /**
     * @brief Parses a variable from a specified input.
     *
     * The input must consist of three characters. For input variables, the
     * first of these must be either `x` or `y`, followed by a two-digit number
     * between `0` and `44` (inclusive). For output variables, the first
     * character must be a `z`. It must also be followed by a two-digit number,
     * only in this case between `0` and `45` (inclusive). For arbitrary
     * variables, no restrictions are placed on the three characters.
     *
     * @param[in] s The input to parse a variable from.
     * @return The parsed variable on success, or `std::nullopt` on failure.
     */
    static constexpr std::optional<Variable> parse(
        std::string_view s
    ) noexcept {
        if (std::ssize(s) != NAME_LENGTH) {
            return std::nullopt;
        }
        if ((s[0] == 'x') || (s[0] == 'y')) {
            if (
                !std::isdigit(static_cast<unsigned char>(s[1]))
                    || !std::isdigit(static_cast<unsigned char>(s[2]))
            ) {
                return std::nullopt;
            }
            isize bitIdx = (s[1] - '0') * 10 + s[2] - '0';
            if (bitIdx > INPUT_LENGTH) {
                return std::nullopt;
            }
        }
        else if (s[0] == 'z') {
            if (
                !std::isdigit(static_cast<unsigned char>(s[1]))
                    || !std::isdigit(static_cast<unsigned char>(s[2]))
            ) {
                return std::nullopt;
            }
            isize bitIdx = (s[1] - '0') * 10 + s[2] - '0';
            if (bitIdx > INPUT_LENGTH + 1) {
                return std::nullopt;
            }
        }
        std::array<char, NAME_LENGTH> name = { s[0], s[1], s[2] };
        return Variable(std::move(name));
    }

    /**
     * @brief Determines if two variables are equal.
     *
     * @param[in] lhs The first variable.
     * @param[in] rhs The second variable.
     * @return `true` if the two variables are equal, otherwise `false`.
     */
    friend constexpr bool operator==(
        const Variable& lhs,
        const Variable& rhs
    ) noexcept {
        return lhs._name == rhs._name;
    }

    /**
     * @brief Returns the name of this variable.
     *
     * @return The name of this variable.
     */
    constexpr std::string_view name() const noexcept {
        return std::string_view(_name.begin(), _name.end());
    }

    /**
     * @brief Determines whether this variable is an input variable.
     *
     * @return `true` if this variable is an input variable, otherwise `false`.
     */
    constexpr bool is_input() const noexcept {
        return (_name[0] == 'x') || (_name[0] == 'y');
    }

    /**
     * @brief Determines whether this variable is an output variable.
     *
     * @return `true` if this variable is an output variable, otherwise `false`.
     */
    constexpr bool is_output() const noexcept {
        return _name[0] == 'z';
    }

    /**
     * @brief Returns the index of the bit this variable corresponds to.
     *
     * @warning If this variable is not an input or output variable, calling
     * this function results in undefined behavior.
     * @return The index of the bit this variable corresponds to.
     */
    constexpr isize bit_idx() const noexcept {
        assert(is_input() || is_output());
        return (_name[1] - '0') * 10 + _name[2] - '0';
    }

};

template<>
struct std::hash<Variable> {
    constexpr usize operator()(const Variable& variable) const noexcept {
        usize seed = 0;
        for (char c : variable.name()) {
            seed ^= std::hash<char>()(c) + 0x9E3779B9 + (seed << 6)
                + (seed >> 2);
        }
        return seed;
    }
};

/** @brief Represents a circuit of logic gates. */
class Circuit final {
private:

    /** @brief Represents the type of a logic gate. */
    enum class GateType {
        AND,
        OR,
        XOR
    };

    /** @brief Represents a logic gate. */
    struct Gate {

        /** @brief The type of this logic gate. */
        GateType type;

        /** @brief The left input variable. */
        Variable lhs;

        /** @brief The right input variable. */
        Variable rhs;

        /**
         * @brief Parses a gate from a specified input.
         *
         * The input must have the following format:
         *
         * ```plaintext
         * <lhs> <op> <rhs>
         * ```
         *
         * Here, `<lhs>` and `<rhs>` represent the input variables of the gate,
         * which must have the format as described in `Variable::parse()`.
         * Additionally, `<lhs>` and `<rhs>` are not allowed to be output
         * variables (i.e., variables whose name starts with a `z`). `<op>` is
         * the type of the gate and must be either `ADD`, `OR` or `XOR`.
         *
         * @param[in] s The input to parse a gate from.
         * @return The parsed gate on success, or `std::nullopt` on failure.
         */
        static constexpr std::optional<Gate> parse(std::string_view s) {
            auto spaceIdx = s.find(' ');
            if (spaceIdx == std::string_view::npos) {
                return std::nullopt;
            }
            std::optional<Variable> lhs = Variable::parse(
                s.substr(0, spaceIdx)
            );
            if (!lhs || lhs->is_output()) {
                return std::nullopt;
            }
            s = s.substr(spaceIdx + 1);
            spaceIdx = s.find(' ');
            if (spaceIdx == std::string_view::npos) {
                return std::nullopt;
            }
            std::string_view op = s.substr(0, spaceIdx);
            GateType type;
            if (op == "AND") {
                type = GateType::AND;
            }
            else if (op == "OR") {
                type = GateType::OR;
            }
            else if (op == "XOR") {
                type = GateType::XOR;
            }
            else {
                return std::nullopt;
            }
            std::optional<Variable> rhs = Variable::parse(
                s.substr(spaceIdx + 1)
            );
            if (!rhs || rhs->is_output()) {
                return std::nullopt;
            }
            return Gate { type, *lhs, *rhs };
        }

    };

    /** @brief The input variables of this circuit. */
    std::unordered_map<Variable, bool> _inputs;

    /** @brief The combinatorial logic of this circuit. */
    std::unordered_map<Variable, Gate> _logic;

    /**
     * @brief Initializes a new circuit with the specified inputs and
     * combinatorial logic.
     *
     * @param[in] inputs The input variables of the circuit.
     * @param[in] logic  The combinatorial logic of the circuit.
     */
    constexpr Circuit(
        std::unordered_map<Variable, bool> inputs,
        std::unordered_map<Variable, Gate> logic
    ) noexcept
        : _inputs(std::move(inputs)),
          _logic(std::move(logic)) { }

public:

    /**
     * @brief Parses a circuit from the standard input stream.
     *
     * The input must consist of one or more lines of text with the following
     * format:
     *
     * ```plaintext
     * <inputs>
     *
     * <logic>
     * ```
     *
     * Here, `<inputs>` represents the input variables of the circuit. Each
     * variable must be on its own line and have the format `<id>: <value>`,
     * where `<id>` starts with either `x` or `y`, followed by a two-digit
     * number. `<value>` must be either `0` or `1`. The `<inputs>` section is
     * separated from the `<logic>` section using an empty line. Within that
     * section, each line represents a logic gate combining two variables (which
     * need not necessarily be both input variables from the `<inputs>`
     * section). Each line must have the format `<id> <op> <id> -> <id>`, where
     * `<op>` specifies the logic gate to be applied (either `AND`, `OR` or
     * `XOR`). The same restrictions mentioned earlier apply for `<id>`, but
     * additionally, the last `<id>` may start with a `z`, which indicates an
     * output.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * x00: 1
     * x01: 1
     * x02: 1
     * y00: 0
     * y01: 1
     * y02: 0
     *
     * x00 AND y00 -> z00
     * x01 XOR y01 -> z01
     * x02 OR y02 -> z02
     * ```
     *
     * @return The parsed circuit on success, or `std::nullopt` on failure.
     */
    static std::optional<Circuit> parse() {
        std::unordered_map<Variable, bool> inputs;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }
            auto separatorIdx = line.find(": ");
            if (separatorIdx == std::string::npos) {
                return std::nullopt;
            }
            std::optional<Variable> variable = Variable::parse(
                std::string_view(line.begin(), line.begin() + separatorIdx)
            );
            if (!variable || !variable->is_input()) {
                return std::nullopt;
            }
            std::string_view rest(line.begin() + separatorIdx + 2, line.end());
            if ((std::ssize(rest) != 1) || (rest[0] < '0') || (rest[0] > '1')) {
                return std::nullopt;
            }
            bool value = rest[0] == '1';
            inputs[*variable] = value;
        }
        std::unordered_map<Variable, Gate> logic;
        while (std::getline(std::cin, line)) {
            auto separatorIdx = line.find(" -> ");
            if (separatorIdx == std::string::npos) {
                return std::nullopt;
            }
            std::optional<Gate> gate = Gate::parse(
                std::string_view(line.begin(), line.begin() + separatorIdx)
            );
            if (!gate) {
                return std::nullopt;
            }
            std::optional<Variable> variable = Variable::parse(
                std::string_view(line.begin() + separatorIdx + 4, line.end())
            );
            if (!variable) {
                return std::nullopt;
            }
            logic.insert_or_assign(*variable, *gate);
        }
        return Circuit(std::move(inputs), std::move(logic));
    }

    /**
     * @brief Returns the output of this circuit as a single number.
     *
     * @return The output of this circuit.
     */
    isize output() const {
        std::unordered_map<Variable, bool> cache = _inputs;
        auto eval = [this, &cache](
            this const auto& self,
            const Variable& variable
        ) -> bool {
            auto it = cache.find(variable);
            if (it != cache.end()) {
                return it->second;
            }
            const Gate& gate = _logic.at(variable);
            switch (gate.type) {
                case GateType::AND:
                    cache[variable] = self(gate.lhs) && self(gate.rhs);
                    break;
                case GateType::OR:
                    cache[variable] = self(gate.lhs) || self(gate.rhs);
                    break;
                case GateType::XOR:
                    cache[variable] = self(gate.lhs) ^ self(gate.rhs);
                    break;
                default:
                    std::unreachable();
            }
            return cache[variable];
        };
        usize output = 0;
        for (const Variable& variable : std::views::keys(_logic)) {
            if (variable.is_output() && eval(variable)) {
                output |= 1ULL << variable.bit_idx();
            }
        }
        return static_cast<isize>(output);
    }

    /**
     * @brief Returns the swapped wires (sorted lexicographically and separated
     * with commas) that cause this circuit to malfunction.
     *
     * @warning This function assumes that this circuit represents a
     * ripple-carry adder for two 44-bit integers. If this assumption is not
     * met, calling this function results in undefined behavior.
     *
     * @return The swapped wires (sorted lexicographically and separated with
     * commas).
     */
    std::string swapped_wires() const {
        auto compare_variables = [](
            const Variable& lhs,
            const Variable& rhs
        ) -> bool {
            return lhs.name() < rhs.name();
        };
        std::set<Variable, decltype(compare_variables)> swappedWires(
            compare_variables
        );
        for (const auto& [variable, gate] : _logic) {
            if (
                variable.is_output()
                    && (variable.bit_idx() != Variable::INPUT_LENGTH + 1)
                    && (gate.type != GateType::XOR)
            ) {
                swappedWires.insert(variable);
            }
            if (
                !variable.is_output()
                    && !gate.lhs.is_input()
                    && !gate.rhs.is_input()
                    && (gate.type == GateType::XOR)
            ) {
                swappedWires.insert(variable);
            }
            if (
                (gate.type == GateType::XOR)
                    && gate.lhs.is_input() && (gate.lhs.bit_idx() != 0)
                    && gate.rhs.is_input() && (gate.rhs.bit_idx() != 0)
            ) {
                auto gates = std::views::values(_logic);
                auto it = std::ranges::find_if(
                    gates,
                    [&variable](const Gate& other) {
                        return (other.type == GateType::XOR)
                            && ((other.lhs == variable)
                                || (other.rhs == variable));
                    }
                );
                if (it == gates.end()) {
                    swappedWires.insert(variable);
                }
            }
            if (
                (gate.type == GateType::AND)
                    && gate.lhs.is_input() && (gate.lhs.bit_idx() != 0)
                    && gate.rhs.is_input() && (gate.rhs.bit_idx() != 0)
            ) {
                auto gates = std::views::values(_logic);
                auto it = std::ranges::find_if(
                    gates,
                    [&variable](const Gate& other) {
                        return (other.type == GateType::OR)
                            && ((other.lhs == variable)
                                || (other.rhs == variable));
                    }
                );
                if (it == gates.end()) {
                    swappedWires.insert(variable);
                }
            }
        }
        std::string result;
        for (const Variable& swappedWire : swappedWires) {
            if (!result.empty()) {
                result += ',';
            }
            result += swappedWire.name();
        }
        return result;
    }

};

int main() {
    std::optional<Circuit> circuit = Circuit::parse();
    if (!circuit) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize output = circuit->output();
    std::string swappedWires = circuit->swapped_wires();
    std::println("The output of the circuit is {}.", output);
    std::println("The swapped wires are '{}'.", swappedWires);
    return EXIT_SUCCESS;
}