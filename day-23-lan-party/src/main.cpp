#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <generator>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <unordered_map>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a node in a network. */
using Node = std::array<char, 2>;

template<>
struct std::hash<Node> {
    usize operator()(const Node& node) const noexcept {
        usize seed = 0;
        seed ^= std::hash<char>()(node[0]) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        seed ^= std::hash<char>()(node[1]) + 0x9E3779B9 + (seed << 6)
            + (seed >> 2);
        return seed;
    }
};

/** @brief Represents a network of inter-connected computers. */
class Network final {
private:

    /** @brief A map of this network. */
    std::unordered_map<Node, std::set<Node>> _neighbors;

    /**
     * @brief Initializes a new network with a specified map.
     *
     * @param[in] neighbors A map of the network.
     */
    constexpr explicit Network(
        std::unordered_map<Node, std::set<Node>> neighbors
    ) noexcept
        : _neighbors(std::move(neighbors)) { }

    /**
     * @brief Returns the maximal clique (i.e., a subset of nodes in which all
     * distinct nodes are connected and that cannot be extended by further
     * nodes) based on a current clique, a set of candidate nodes, as well as
     * a set of excluded nodes.
     *
     * @param[in] clique     The current clique.
     * @param[in] candidates The candidates for extending the clique.
     * @param[in] excluded   The nodes that should not be considered for
     *                       extending the clique.
     * @return The maximal clique based on the current state.
     */
    std::generator<std::set<Node>> bron_kerbosch(
        const std::set<Node>& clique,
        std::set<Node> candidates,
        std::set<Node> excluded
    ) const {
        if (candidates.empty() && excluded.empty()) {
            if (std::ssize(clique) > 2) {
                co_yield clique;
            }
            co_return;
        }
        std::set<Node> pivotCandidates = candidates;
        pivotCandidates.insert_range(excluded);
        const Node& pivot = *std::ranges::max_element(
            pivotCandidates,
            [this](const Node& lhs, const Node& rhs) {
                return _neighbors.at(lhs) < _neighbors.at(rhs);
            }
        );
        std::set<Node> remainingCandidates;
        std::ranges::set_difference(
            candidates,
            _neighbors.at(pivot),
            std::inserter(remainingCandidates, remainingCandidates.begin())
        );
        for (const Node& candidate : remainingCandidates) {
            std::set<Node> newClique = clique;
            newClique.insert(candidate);
            std::set<Node> newCandidates;
            std::ranges::set_intersection(
                candidates,
                _neighbors.at(candidate),
                std::inserter(newCandidates, newCandidates.begin())
            );
            std::set<Node> newExcluded;
            std::ranges::set_intersection(
                excluded,
                _neighbors.at(candidate),
                std::inserter(newExcluded, newExcluded.begin())
            );
            for (
                const std::set<Node>& result
                    : bron_kerbosch(newClique, newCandidates, newExcluded)
            ) {
                co_yield result;
            }
            candidates.erase(candidate);
            excluded.insert(candidate);
        }
    }

public:

    /**
     * @brief Parses a network from the standard input stream.
     *
     * The input must consist of zero or more lines of text, where each line
     * specifies a connection between two nodes (i.e., computers) in the
     * following format:
     *
     * ```plaintext
     * <node a>-<node b>
     * ```
     *
     * Here, `<node a>` and `<node b>` represent the identifiers of the nodes,
     * which must consist of two lowercase letters. An example for a valid
     * input might be the following:
     *
     * ```plaintext
     * kh-tc
     * qp-kh
     * de-cg
     * ka-co
     * yn-aq
     * ```
     *
     * @return The parsed network on success, or `std::nullopt` on failure.
     */
    static std::optional<Network> parse() {
        std::unordered_map<Node, std::set<Node>> neighbors;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (
                (std::ssize(line) != 5)
                    || !std::islower(static_cast<unsigned char>(line[0]))
                    || !std::islower(static_cast<unsigned char>(line[1]))
                    || (line[2] != '-')
                    || !std::islower(static_cast<unsigned char>(line[3]))
                    || !std::islower(static_cast<unsigned char>(line[4]))
            ) {
                return std::nullopt;
            }
            Node left = { line[0], line[1] };
            Node right = { line[3], line[4] };
            neighbors[left].insert(right);
            neighbors[right].insert(left);
        }
        return Network(std::move(neighbors));
    }

    /**
     * @brief Counts the number of sets of three inter-connected nodes where at
     * least of the identifiers starts with `t`.
     *
     * @return The number of relevant sets of three inter-connected nodes.
     */
    isize count_sets_of_three() const {
        isize sets = 0;
        for (const auto& [u, neighbors] : _neighbors) {
            for (
                const auto& [v, w]
                    : std::views::cartesian_product(neighbors, neighbors)
            ) {
                if (
                    (u < v)
                        && (v < w)
                        && std::ranges::contains(_neighbors.at(v), w)
                        && ((u[0] == 't') || (v[0] == 't') || (w[0] == 't'))
                ) {
                    sets++;
                }
            }
        }
        return sets;
    }

    /**
     * @brief Returns the password to get into the LAN party.
     *
     * The password of the LAN party corresponds to the name of all
     * participating computers, sorted lexicographically and joined using
     * commas.
     *
     * @return The password to get into the LAN party.
     */
    std::string password() const {
        std::set<Node> maxClique;
        std::set<Node> clique;
        std::set<Node> candidates(
            std::from_range,
            std::views::keys(_neighbors)
        );
        std::set<Node> excluded;
        for (
            const std::set<Node>& result
                : bron_kerbosch(clique, candidates, excluded)
        ) {
            if (std::ssize(result) > std::ssize(maxClique)) {
                maxClique = result;
            }
        }
        std::string password;
        for (const Node& node : maxClique) {
            if (!password.empty()) {
                password += ',';
            }
            password += node[0];
            password += node[1];
        }
        return password;
    }

};

int main() {
    std::optional<Network> network = Network::parse();
    if (!network) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    isize sets = network->count_sets_of_three();
    std::string password = network->password();
    std::println("There are {} relevant sets of three computers.", sets);
    std::println("The password to get into the LAN party is '{}'.", password);
    return EXIT_SUCCESS;
}