project(
  'lavat',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

fs = import('fs')
if not fs.exists(meson.current_source_dir() / 'lib/printer/meson.build')
  message('The printer submodule is missing!')
  message('Please run: git submodule update --init --recursive')
  message('Or download the library independently')
endif

subdir('lib/printer')
dependencies = [
  printer_dep,
]

sources = [
  'src/main.c',
  'src/ball.c',
]

exe = executable(
  'lavat',
  sources,
  dependencies : dependencies,
  install : true,
)
