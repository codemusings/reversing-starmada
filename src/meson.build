executable('zpipe', 'zpipe.c', dependencies: [dependency('zlib')])
executable('zfs_fileutil',
    sources: [
        'zfs_fileutil.c',
        'zfs_free.c',
        'zfs_read.c'
    ],
    dependencies: [
        dependency('gmp'),
        dependency('nettle'),
        dependency('hogweed'),
        dependency('zlib')
    ]
)

executable('sod_dirutil', sources: ['dirutil.c', 'sod.c', 'sod_read.c'])
executable('sod_fileutil', sources: ['fileutil.c', 'sod.c', 'sod_free.c', 'sod_print.c', 'sod_read.c'])
executable('sod_nodeinspector', sources: ['nodeinspector.c', 'sod.c', 'sod_free.c', 'sod_read.c'])
executable('sod_test', sources: ['test.c', 'sod.c', 'sod_free.c', 'sod_print.c', 'sod_read.c'])

cpp = meson.get_compiler('cpp')
sources = [
    'glad.c',
    'model.cpp',
    'shader.cpp',
    'sod.c',
    'sod_free.c',
    'sod_print.c',
    'sod_read.c',
    'viewer.cpp'
]
dependencies = [
    cpp.find_library('glfw'),
    dependency('gl'),
    cpp.find_library('X11'),
    cpp.find_library('pthread'),
    cpp.find_library('Xrandr'),
    cpp.find_library('Xi'),
    cpp.find_library('dl')
]
executable('sod_viewer',
    sources: sources,
    dependencies: dependencies,
    include_directories: includes)
