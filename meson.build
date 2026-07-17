project(
    'advent-of-code-2024',
    'cpp',
    default_options: [
        'cpp_std=c++23',
        'buildtype=debug',
        'b_ndebug=if-release',
        'warning_level=3',
    ],
    meson_version: '>=1.1',
)

cpp = meson.get_compiler('cpp')
deps = []

if cpp.get_id() == 'gcc'
    add_project_arguments(
        '-Wundef',
        '-Wshadow=local',
        '-Wconversion',
        '-Wdouble-promotion',
        '-Wcast-align',
        '-Wcast-qual',
        '-Wlogical-op',
        '-Wpointer-arith',
        '-Wformat=2',
        '-Wnrvo',
        '-Wswitch-default',
        '-Wswitch-enum',
        '-Wuseless-cast',
        '-Wmissing-declarations',
        '-Wmissing-include-dirs',
        '-Wduplicated-cond',
        '-Wduplicated-branches',
        '-Wzero-as-null-pointer-constant',
        '-Wunused-macros',
        '-Wenum-conversion',
        '-Wflex-array-member-not-at-end',
        '-Wredundant-decls',
        language: 'cpp',
    )
    if get_option('native')
        add_project_arguments('-march=native', '-mtune=native', language: 'cpp')
    endif
    # Required for some new C++23 features like `std::println()`.
    deps += cpp.find_library('stdc++exp', required: true)
endif

deps += cpp.find_library('m', required: false)

sh = find_program('sh')

days = [
    'day-01-historian-hysteria',
    'day-02-red-nosed-reports',
    'day-03-mull-it-over',
    'day-04-ceres-search',
    'day-05-print-queue',
    'day-06-guard-gallivant',
    'day-07-bridge-repair',
    'day-08-resonant-collinearity',
    'day-09-disk-fragmenter',
    'day-10-hoof-it',
    'day-11-plutonian-pebbles',
    'day-12-garden-groups',
    'day-13-claw-contraption',
    'day-14-restroom-redoubt',
    'day-15-warehouse-woes',
    'day-16-reindeer-maze',
    'day-17-chronospatial-computer',
]

incs = include_directories('include')

foreach day : days
    exe = executable(
        day,
        day / 'src' / 'main.cpp',
        include_directories: incs,
        dependencies: deps,
    )
    run_target(
        'run-' + day,
        command: [
            sh,
            '-c',
            exe.full_path()
                + ' < '
                + meson.project_source_root() / day / 'resources' / 'input.txt',
        ],
        depends: exe,
    )
endforeach