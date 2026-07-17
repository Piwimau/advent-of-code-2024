#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <optional>
#include <print>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a simple 3-bit CPU. */
struct Cpu {

    /** @brief The value of the A register. */
    isize a;

    /** @brief The value of the B register. */
    isize b;

    /** @brief The value of the C register. */
    isize c;

    /** @brief The value of the program counter. */
    isize pc;

};

/** @brief Represents the opcode of an instruction. */
enum class Opcode {

    /**
     * @brief Divides the value of the A register by two to the power of the
     * instruction's combo operand and stores the result back into the A
     * register.
     */
    Adv,

    /**
     * @brief Calculates the bitwise XOR of the value of the B register and the
     * instruction's literal operand and stores the result back into the B
     * register.
     */
    Bxl,

    /**
     * @brief Calculates the value of the instruction's combo operand modulo 8
     * and stores the result in the B register.
     */
    Bst,

    /**
     * @brief If the value of the A register is not 0, jumps to the instruction
     * specified by the instruction's literal operand. Consequently, the
     * instruction pointer is not incremented if the jump is executed.
     */
    Jnz,

    /**
     * @brief Calculates the bitwise XOR of the registers B and C and stores the
     * result back into the B register. This instruction reads an operand, but
     * implicitly ignores (for historical reasons).
     */
    Bxc,

    /**
     * @brief Calculates the value of the instruction's combo operand modulo 8
     * and outputs the result.
     */
    Out,

    /**
     * @brief Divides the value of the A register by two to the value of the
     * instruction's combo operand and stores the result in the B register.
     */
    Bdv,

    /**
     * @brief Divides the value of the A register by two to the value of the
     * instruction's combo operand and stores the result in the C register.
     */
    Cdv

};

/** @brief Represents an operand of an instruction. */
class Operand final {
private:

    /** @brief Represents a literal operand. */
    using Literal = isize;

    /** @brief Represents a register operand. */
    enum class Register {
        A,
        B,
        C
    };

    /** @brief The actual operand of the instruction. */
    std::variant<Literal, Register> _operand;

    /**
     * @brief Initializes a new operand.
     *
     * @param[in] operand The actual operand of the instruction.
     */
    explicit Operand(std::variant<Literal, Register> operand) noexcept
        : _operand(operand) { }

public:

    /**
     * @brief Parses an operand from an input.
     *
     * @param[in] opcode The opcode to which the operand belongs.
     * @param[in] input  The input to parse.
     * @return The parsed operand on success, otherwise `std::nullopt`.
     */
    static std::optional<Operand> parse(Opcode opcode, isize input) noexcept {
        if ((input < 0) || (input > 7)) {
            return std::nullopt;
        }
        switch (opcode) {
            case Opcode::Bxl:
            case Opcode::Jnz:
            case Opcode::Bxc:
                return Operand(input);
            case Opcode::Adv:
            case Opcode::Bst:
            case Opcode::Out:
            case Opcode::Bdv:
            case Opcode::Cdv:
                switch (input) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        return Operand(input);
                    case 4:
                        return Operand(Register::A);
                    case 5:
                        return Operand(Register::B);
                    case 6:
                        return Operand(Register::C);
                    default:
                        return std::nullopt;
                }
            default:
                std::unreachable();
        }
    }

    /**
     * @brief Evaluates this operand.
     *
     * @param[in] cpu The state of the CPU.
     * @return The result of evaluating this operand.
     */
    isize eval(const Cpu& cpu) const noexcept {
        return std::visit(
            [&cpu](auto&& arg) -> isize {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Literal>) {
                    return arg;
                }
                else {
                    switch (arg) {
                        case Register::A:
                            return cpu.a;
                        case Register::B:
                            return cpu.b;
                        case Register::C:
                            return cpu.c;
                        default:
                            std::unreachable();
                    }
                }
            },
            _operand
        );
    }

    /**
     * @brief Converts this operand to the integer representation.
     *
     * @return The integer representation of this operand.
     */
    operator isize() const noexcept {
        return std::visit(
            [](auto&& arg) -> isize {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Literal>) {
                    return arg;
                }
                else {
                    switch (arg) {
                        case Register::A:
                            return 4;
                        case Register::B:
                            return 5;
                        case Register::C:
                            return 6;
                        default:
                            std::unreachable();
                    }
                }
            },
            _operand
        );
    }

};

/** @brief Represents an instruction consisting of an opcode and operand. */
struct Instruction {

    /** @brief The opcode of this instruction. */
    Opcode opcode;

    /** @brief The operand of this instruction. */
    Operand operand;

};

/** @brief Represents a sequence of instructions. */
using Program = std::vector<Instruction>;

/**
 * @brief Parses the input from the standard input stream.
 *
 * The input must consist of five lines with the following format:
 *
 * ```plaintext
 * Register A: <value>
 * Register B: <value>
 * Register C: <value>
 *
 * Program: <instructions-and-operands>
 * ```
 *
 * Here, `value` represents the value of the corresponding register as an
 * integer. `<instructions-and-operands>` must be a comma-separated list of
 * integers between `0` and `7` each. The number of integers in that list must
 * be even, as the first integer represents the opcode of an instruction, while
 * the second one represents the corresponding operand.
 *
 * An example for a valid input might be the following:
 *
 * ```plaintext
 * Register A: 729
 * Register B: 0
 * Register C: 0
 *
 * Program: 0,1,5,4,3,0
 * ```
 *
 * @return The parsed input on success, otherwise `std::nullopt`.
 */
static std::optional<std::pair<Cpu, Program>> parse_input() {
    Cpu cpu = { };
    auto parse_register = [](const std::string& name) -> std::optional<isize> {
        std::string line;
        if (!std::getline(std::cin, line)) {
            return std::nullopt;
        }
        isize value;
        std::string format = std::format("Register {}: %td", name);
        if (std::sscanf(line.c_str(), format.c_str(), &value) != 1) {
            return std::nullopt;
        }
        return value;
    };
    std::optional<isize> a = parse_register("A");
    std::optional<isize> b = parse_register("B");
    std::optional<isize> c = parse_register("C");
    if (!a || !b || !c) {
        return std::nullopt;
    }
    cpu.a = *a;
    cpu.b = *b;
    cpu.c = *c;
    std::string line;
    if (!std::getline(std::cin, line) || !line.empty()) {
        return std::nullopt;
    }
    if (!std::getline(std::cin, line) || !line.starts_with("Program: ")) {
        return std::nullopt;
    }
    std::vector<isize> values;
    std::istringstream iss(line.substr(std::size("Program: ") - 1));
    std::string token;
    while (std::getline(iss, token, ',')) {
        isize value;
        auto [ptr, ec] = std::from_chars(
            token.data(),
            token.data() + std::ssize(token),
            value
        );
        if (
            (ec != std::errc())
                || (ptr != token.data() + std::ssize(token))
                || (value < 0)
                || (value > 7)
        ) {
            return std::nullopt;
        }
        values.push_back(value);
    }
    // Each opcode must be followed by an operand, so the number of values must
    // be even.
    if ((std::ssize(values) % 2) != 0) {
        return std::nullopt;
    }
    Program program;
    for (isize i = 0; i < std::ssize(values); i += 2) {
        Opcode opcode = static_cast<Opcode>(values[i]);
        std::optional<Operand> operand = Operand::parse(opcode, values[i + 1]);
        if (!operand) {
            return std::nullopt;
        }
        program.emplace_back(opcode, *operand);
    }
    return std::make_pair(cpu, program);
}

/**
 * @brief Executes a program on a specified CPU.
 *
 * @param[in] cpu     The CPU to execute the program on.
 * @param[in] program The program to execute.
 * @return The output produced by the program.
 */
static std::vector<isize> exec(Cpu cpu, const Program& program) {
    std::vector<isize> output;
    while (cpu.pc < std::ssize(program)) {
        const auto& [opcode, operand] = program[cpu.pc];
        switch (opcode) {
            case Opcode::Adv:
                cpu.a /= 1 << operand.eval(cpu);
                cpu.pc++;
                break;
            case Opcode::Bxl:
                cpu.b ^= operand.eval(cpu);
                cpu.pc++;
                break;
            case Opcode::Bst:
                cpu.b = operand.eval(cpu) % 8;
                cpu.pc++;
                break;
            case Opcode::Jnz:
                if (cpu.a != 0) {
                    // We divide the operand by two, as we already parsed the
                    // pairs of integers into actual instructions. We therefore
                    // only need to adjust the program counter in steps of one.
                    cpu.pc = operand.eval(cpu) / 2;
                }
                else {
                    cpu.pc++;
                }
                break;
            case Opcode::Bxc:
                cpu.b ^= cpu.c;
                cpu.pc++;
                break;
            case Opcode::Out:
                output.push_back(operand.eval(cpu) % 8);
                cpu.pc++;
                break;
            case Opcode::Bdv:
                cpu.b = cpu.a / (1 << operand.eval(cpu));
                cpu.pc++;
                break;
            case Opcode::Cdv:
                cpu.c = cpu.a / (1 << operand.eval(cpu));
                cpu.pc++;
                break;
            default:
                std::unreachable();
        }
    }
    return output;
}

/**
 * @brief Finds the lowest possible initial value of the A register of a CPU
 * such that the specified program is a quine (i.e., it produces itself as the
 * output).
 *
 * @param[in] cpu     The CPU to execute the program on.
 * @param[in] program The program to execute.
 * @return The lowest initial value of the A register.
 */
static isize find_initial_a_for_quine(const Cpu& cpu, const Program& program) {
    std::vector<isize> expected;
    for (const Instruction& instruction : program) {
        const auto& [opcode, operand] = instruction;
        expected.push_back(static_cast<isize>(opcode));
        expected.push_back(static_cast<isize>(operand));
    }
    auto search = [&cpu, &program, &expected](
        this const auto& self,
        isize a,
        isize idx
    ) -> std::optional<isize> {
        if (idx < 0) {
            return a;
        }
        for (isize digit = 0; digit < 8; digit++) {
            isize candidate = (a << 3) | digit;
            std::vector<isize> actual = exec(
                Cpu { .a = candidate, .b = cpu.b, .c = cpu.c, .pc = cpu.pc },
                program
            );
            if (
                (std::ssize(actual) == std::ssize(expected) - idx)
                    && std::equal(
                        actual.begin(),
                        actual.end(),
                        expected.begin() + idx
                    )
            ) {
                std::optional<isize> result = self(candidate, idx - 1);
                if (result) {
                    return result;
                }
            }
        }
        return std::nullopt;
    };
    return *search(0, std::ssize(expected) - 1);
}

/**
 * @brief Formats the output of a program as comma-separated list.
 *
 * @param[in] output The output to format.
 * @return The formatted output.
 */
static std::string format_output(const std::vector<isize>& output) {
    std::stringstream s;
    for (isize i = 0; i < std::ssize(output); i++) {
        s << output[i];
        if (i < std::ssize(output) - 1) {
            s << ',';
        }
    }
    return s.str();
}

int main() {
    std::optional<std::pair<Cpu, Program>> input = parse_input();
    if (!input) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    auto [cpu, program] = *input;
    std::string output = format_output(exec(cpu, program));
    isize a = find_initial_a_for_quine(cpu, program);
    std::println("The output is '{}'.", output);
    std::println("The lowest initial value of the A register must be {}.", a);
    return EXIT_SUCCESS;
}