project('2dtest', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

curses = dependency('curses', method: 'pkg-config')

executable('2dtest',
           'main.cpp',
           dependencies: curses,
           include_directories: curses.get_pkgconfig_variable('includedir'),
           install : true)
