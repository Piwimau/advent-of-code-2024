# 🎄 Advent of Code 2024 🎄

This repository contains my solutions for [Advent of Code
2024](https://adventofcode.com/2024), my fourth year of participation.

## What Is Advent of Code?

[Advent of Code](https://adventofcode.com/) is a series of small programming
puzzles created by [Eric Wastl](http://was.tl/). Every day from December 1st to
25th, a puzzle is released alongside an engaging fictional Christmas story. Each
puzzle consists of two parts, the second of which usually contains some
interesting twist or changing requirements and is only unlocked after completing
the first one. The objective is to solve all parts and collect fifty stars ⭐
until December 25th to save Christmas.

Many users compete on the [global
leaderboard](https://adventofcode.com/2024/leaderboard) by solving the puzzles
in an unbelievably fast way in order to get some extra points. Personally, I see
Advent of Code as a fun exercise to do during the Advent season while waiting
for Christmas. I often use it to learn a new programming language (like I did in
2021 with `C#`) or some advanced programming concepts. I can only encourage you
to participate as well – of course in a way that you find fun. Just get started
and learn more about Advent of Code [here](https://adventofcode.com/2024/about).

## About This Project

For this year of Advent of Code, I decided to go with `C++`. In the previous
year, I had a lot of fun solving the puzzles in `C` and implementing my own
small utility library called [`SCU`](https://github.com/Piwimau/scu) alongside.
It was a great opportunity to improve my systems programming skills, and I also
enjoyed the challenge of working with a rather low-level language. However, I
have always been interested in `C++` as well, but felt a bit intimidated by its
complexity (or at least how complex it is often portrayed to be). This year, I
finally decided to give it a try and see how it compares to `C`, especially in
terms of development speed, the improved type safety and advanced language
features like templates, operator overloading or RAII, which I sometimes miss
when working with `C`.

For this project and in general when developing software, I strive to produce
readable and well-documented source code. However, I also enjoy benchmarking and
optimizing my code, which is why I sometimes implement a less idiomatic, yet
more efficient solution at the expense of readability. In such situations, I try
to document my design choices with analogies, possible alternative solutions and
sometimes little sketches to better illustrate the way a piece of code works.

The general structure of this project is as follows:

```plaintext
day-01-historian-hysteria/
  resources/
    .gitkeep
  src/
    main.cpp
day-02-red-nosed-reports/
  resources/
    .gitkeep
  src/
    main.cpp
...
day-25-code-chronicle/
  ...
include/
  ...
.clang-format
.gitignore
LICENSE
meson.build
meson.options
README.md
```

The repository contains 25 standalone projects for the days of the Advent
calendar, organized into separate directories. Each one provides a `src`
directory with a `main.cpp` file in which the execution begins. In addition,
there is a `resources` directory which contains the puzzle description and my
personal input for that day. However, [as
requested](https://adventofcode.com/2024/about) by the creator of Advent of
Code, these are only present in my own private copy of the repository and
therefore not publicly available.

> If you're posting a code repository somewhere, please don't include parts of
> Advent of Code like the puzzle text or your inputs.

As a consequence, you will have to provide your own inputs for the days, as
described in more detail in the following section.

## Dependencies and Usage

If you want to try out one of my solutions, simply follow these steps below:

1. Ensure you have a compiler supporting the `C++23` standard (such as `GCC`,
   `Clang` or `MSVC`) installed on your machine.

2. Clone the repository (or download the source code) to a directory of your
   choice.

   ```shell
   git clone https://github.com/Piwimau/advent-of-code-2024 ./advent-of-code-2024
   cd ./advent-of-code-2024
   ```

3. Once you have downloaded the source code, you can configure the build
   process:

   ```shell
   meson setup build
   ```

   By default, this will configure the build process to produce unoptimized
   debug executables. To build optimized release executables instead, specify
   the `--buildtype=release` option:

   ```shell
   meson setup build --buildtype=release
   ```

   To build the solution for a specific day, run the following command, where
   `<day>` corresponds to the directory of the day (e.g.,
   `day-01-historian-hysteria`):

   ```shell
   meson compile -C build <day>
   ```

4. To run the solution for a specific day, run the following command:

   ```shell
   meson compile -C build run-<day>
   ```

   Again, `<day>` corresponds to the directory of the day (e.g.,
   `day-01-historian-hysteria`). Note that the solutions read the puzzle input
   from the standard input stream. The `run-<day>` target expects a file called
   `input.txt` in the `resources` directory of the selected day, which is used
   to redirect the standard input stream. [As explained
   above](#about-this-project), my input files are not included in the
   repository, so you'll have to create them yourself and paste your puzzle
   input into them. You can find your input for each day
   [here](https://adventofcode.com/2024) if you haven't downloaded it already.

## Timings

Finally, here are some simple (non-scientific) timings I created using
`std::chrono::high_resolution_clock` and my main machine (Intel Core i9-13900HX,
32GB DDR5-5600 RAM) running Fedora Linux 44 (Workstation Edition). All used
`--buildtype=release` and `-Dnative=true` to take advantage of
(machine-specific) optimizations. The reported times are the result of ten runs
and represent the (real) wall time, including the time spent for parsing the
input, as well as printing the puzzle results.

| Day                             |        Min |        Max |       Mean |     Median | Standard Deviation |
|---------------------------------|-----------:|-----------:|-----------:|-----------:|-------------------:|
| Day 1 – Historian Hysteria      |   1.290 ms |   1.588 ms |   1.399 ms |   1.374 ms |           0.104 ms |
| Day 2 – Red-Nosed Reports       |   1.982 ms |   2.455 ms |   2.260 ms |   2.283 ms |           0.138 ms |
| Day 3 – Mull It Over            |   2.593 ms |   3.051 ms |   2.771 ms |   2.756 ms |           0.136 ms |
| Day 4 – Ceres Search            |   1.454 ms |   1.891 ms |   1.689 ms |   1.716 ms |           0.148 ms |
| Day 5 – Print Queue             |   2.735 ms |   3.131 ms |   2.866 ms |   2.802 ms |           0.120 ms |
| Day 6 – Guard Gallivant         |   4.231 ms |   4.634 ms |   4.385 ms |   4.340 ms |           0.153 ms |
| Day 7 – Bridge Repair           |  12.298 ms |  14.378 ms |  13.176 ms |  13.069 ms |           0.598 ms |
| Day 8 – Resonant Collinearity   |   0.561 ms |   1.030 ms |   0.764 ms |   0.763 ms |           0.127 ms |
| Day 9 – Disk Fragmenter         |   2.421 ms |   3.022 ms |   2.687 ms |   2.654 ms |           0.219 ms |
| Day 10 – Hoof It                |   1.161 ms |   1.467 ms |   1.296 ms |   1.284 ms |           0.089 ms |
| Day 11 – Plutonian Pebbles      |   7.963 ms |  12.116 ms |   9.590 ms |   8.871 ms |           1.597 ms |
| Day 12 – Garden Groups          |   2.191 ms |   2.367 ms |   2.310 ms |   2.317 ms |           0.049 ms |
| Day 13 – Claw Contraption       |   0.507 ms |   0.605 ms |   0.550 ms |   0.548 ms |           0.030 ms |
| Day 14 – Restroom Redoubt       |  51.324 ms |  59.645 ms |  55.791 ms |  55.786 ms |           2.630 ms |
| Day 15 – Warehouse Woes         |   1.460 ms |   2.423 ms |   1.635 ms |   1.499 ms |           0.299 ms |
| Day 16 – Reindeer Maze          |   9.782 ms |  14.607 ms |  12.041 ms |  11.265 ms |           1.864 ms |
| Day 17 – Chronospatial Computer |   0.148 ms |   0.274 ms |   0.217 ms |   0.213 ms |           0.033 ms |
| Day 18 – RAM Run                |   1.266 ms |   1.819 ms |   1.395 ms |   1.351 ms |           0.160 ms |
| Total                           | 105.367 ms | 130.503 ms | 116.820 ms | 114.888 ms |           8.495 ms |

> [!NOTE]
>
> The timings shown above represent actual computation times, not total process
> lifetimes. Startup and shutdown of processes usually introduces an overhead of
> a few milliseconds, but since this is not relevant for the performance of the
> solutions themselves, I decided to ignore it here.

## License

This project is licensed under the [MIT License](LICENSE). Feel free to
experiment with the code, adapt it to your own preferences, and share it with
others.