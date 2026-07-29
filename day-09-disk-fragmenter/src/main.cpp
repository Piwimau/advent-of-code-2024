#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <queue>
#include <utility>
#include <vector>
#include "aoc/types.hpp"

using namespace aoc;

/** @brief Represents a disk of allocated and free blocks. */
class Disk final {
private:

    /** @brief Represents a block on a disk. */
    struct Block {

        /** @brief The maximum size of a block. */
        static constexpr isize MAX_SIZE = 9;

        /** @brief The ID of the block, or `-1` if it is not allocated. */
        isize id;

        /** @brief The size of the block. */
        isize size;

        /**
         * @brief Determines whether this block is allocated or not.
         *
         * @return `true` if this block is allocated, otherwise `false`.
         */
        bool is_allocated() const noexcept {
            return id != -1;
        }

    };

    /** @brief The blocks on the disk. */
    std::vector<Block> _blocks;

    /**
     * @brief Initializes a new disk with the specified blocks.
     *
     * @param[in] blocks The blocks on the disk.
     */
    Disk(std::vector<Block> blocks) : _blocks(std::move(blocks)) { }

public:

    /**
     * @brief Parses a disk from the standard input stream.
     *
     * The input must consist of a sequence of digits, where each digit
     * represents the size of a block. The allocated and free blocks must be
     * interleaved, i.e., the first block is interpreted as allocated, the
     * second block as free, the third block as allocated, and so on.
     *
     * An example for a valid input might be the following:
     *
     * ```plaintext
     * 2333133121414131402
     * ```
     *
     * @note Blocks of size `0` are not stored in the resulting disk, regardless
     * of whether they are allocated or not.
     *
     * @return The parsed disk on success, or `std::nullopt` on failure.
     */
    static std::optional<Disk> parse() {
        std::vector<Block> blocks;
        isize id = 0;
        char c;
        for (isize i = 0; std::cin.get(c); i++) {
            if ((c < '0') || (c > '9')) {
                return std::nullopt;
            }
            isize size = c - '0';
            if (size > 0) {
                bool isAllocated = (i % 2) == 0;
                blocks.emplace_back(isAllocated ? id++ : -1, size);
            }
        }
        return Disk(std::move(blocks));
    }

    /**
     * @brief Compacts this disk.
     *
     * @note This method may cause fragmentation, as blocks are moved from the
     * right to the left one by one and split if they do not fit into a free
     * block.
     */
    void compact() {
        std::vector<isize> flat;
        for (const Block& block : _blocks) {
            flat.insert(flat.end(), block.size, block.id);
        }
        isize l = 0;
        isize r = std::ssize(flat) - 1;
        while (l < r) {
            while ((l < r) && (flat[l] != -1)) {
                l++;
            }
            while ((l < r) && (flat[r] == -1)) {
                r--;
            }
            if (l < r) {
                std::swap(flat[l++], flat[r--]);
            }
        }
        _blocks.clear();
        for (isize begin = 0; begin < std::ssize(flat);) {
            isize end = begin;
            while ((end < std::ssize(flat)) && (flat[begin] == flat[end])) {
                end++;
            }
            isize id = flat[begin];
            isize size = end - begin;
            _blocks.emplace_back(id, size);
            begin = end;
        }
    }

    /**
     * @brief Compacts this disk without causing fragmentation.
     *
     * @note In contrast to `compact()`, this method moves entire blocks from
     * the right to the left, i.e., it does not split blocks if they do not fit
     * into a free block. This means that the resulting disk will not suffer
     * from fragmentation, but it may not be as compact as the one produced by
     * `compact()`.
     */
    void compact_without_fragmentation() {
        struct Entry {
            isize idx;
            isize id;
            isize size;

            bool operator>(const Entry& other) const noexcept {
                return idx > other.idx;
            }
        };
        auto max = std::ranges::max_element(
            _blocks,
            std::less<isize>(),
            &Block::id
        );
        if (max == _blocks.end()) {
            return;
        }
        isize maxId = max->id;
        std::vector<Entry> allocated(maxId + 1);
        std::array<
            std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>,
            Block::MAX_SIZE + 1
        > free;
        isize idx = 0;
        for (const Block& block : _blocks) {
            if (block.is_allocated()) {
                allocated[block.id] = { idx, block.id, block.size };
            }
            else {
                free[block.size].emplace(idx, -1, block.size);
            }
            idx += block.size;
        }
        for (isize id = maxId; id >= 0; id--) {
            std::optional<Entry> best;
            for (isize s = allocated[id].size; s <= Block::MAX_SIZE; s++) {
                if (
                    !free[s].empty()
                        && (free[s].top().idx < allocated[id].idx)
                        && (!best || (free[s].top().idx < best->idx))
                ) {
                    best = free[s].top();
                }
            }
            if (!best) {
                continue;
            }
            free[best->size].pop();
            allocated[id].idx = best->idx;
            isize remainder = best->size - allocated[id].size;
            if (remainder > 0) {
                free[remainder].emplace(
                    best->idx + allocated[id].size,
                    -1,
                    remainder
                );
            }
        }
        std::ranges::sort(allocated, std::less<isize>(), &Entry::idx);
        _blocks.clear();
        idx = 0;
        for (const Entry& e : allocated) {
            if (e.idx > idx) {
                _blocks.emplace_back(-1, e.idx - idx);
            }
            _blocks.emplace_back(e.id, e.size);
            idx = e.idx + e.size;
        }
    }

    /**
     * @brief Returns the checksum of this disk.
     *
     * @return The checksum of this disk.
     */
    isize checksum() const noexcept {
        isize checksum = 0;
        isize idx = 0;
        for (const Block& block : _blocks) {
            if (block.is_allocated()) {
                for (isize i = 0; i < block.size; i++) {
                    checksum += idx++ * block.id;
                }
            }
            else {
                idx += block.size;
            }
        }
        return checksum;
    }

};

int main() {
    std::optional<Disk> disk = Disk::parse();
    if (!disk) {
        std::println(stderr, "An error occurred while reading the input file.");
        return EXIT_FAILURE;
    }
    Disk copy = *disk;
    disk->compact();
    copy.compact_without_fragmentation();
    isize checksum = disk->checksum();
    isize checksumWithoutFragmentation = copy.checksum();
    std::println("The checksum of the compacted disk is {}.", checksum);
    std::println(
        "The checksum of the compacted disk without fragmentation is {}.",
        checksumWithoutFragmentation
    );
    return EXIT_SUCCESS;
}