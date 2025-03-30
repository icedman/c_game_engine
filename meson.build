project('game', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

pwd = meson.current_source_dir()
cc = meson.get_compiler('c')

game_files = [
    'src/main.c'
]

engine_files = [
    'engine/platform.c',
    'engine/render.c',
    'engine/utils.c',
    'engine/animation.c',
    'engine/image.c',
    'engine/input.c',
    'engine/trace.c',
    'engine/entity.c',
    'engine/engine.c',
    'engine/font.c',
    'engine/map.c',
    'src/scenes/game.c',
    'src/entities/coin.c',
    'src/entities/player.c',
    'src/main.c',
]

includes = include_directories(
    'engine',
    'libs',
    'src',
)

m_dep = cc.find_library('m', required : true, disabler: true)
gl_dep = dependency('opengl', required: true, disabler: true)
sdl2_dep = dependency('SDL2', required: true, disabler: true)
sdl2_mixer_dep = dependency('SDL2_Mixer', required: true, disabler: true)
sdl2_image_dep = dependency('SDL2_image', required: true, disabler: true)

executable('game',
    engine_files,
    game_files,
    include_directories: [ includes ],
    c_args: ['-DRENDER_SOFTWARE', '-DPLATFORM_SDL'],
    dependencies: [ m_dep, sdl2_dep, sdl2_mixer_dep, sdl2_image_dep, gl_dep ]
)
