from os import environ

# 1. Pastas de Build
VariantDir('build/src', 'src', duplicate=0)
VariantDir('build/lib', 'lib', duplicate=0)

flags = ['-O3', '-march=native', '-std=c++14']

env = Environment(ENV       = environ,
                  CXX       = 'clang++',
                  CPPFLAGS  = ['-Wno-unused-value'],
                  CXXFLAGS  = flags,
                  LINKFLAGS = flags,
                  CPPPATH   = [
                      '#simpleini', 
                      '#lib/include', 
                      '#src/include', 
                      '#lib/imgui', 
                      '#lib/imgui/backends'
                  ],
                  LIBS      = ['SDL2', 'SDL2_image', 'SDL2_ttf'])

env.ParseConfig('sdl2-config --cflags --libs')

# 2. LISTA DE FONTES BASEADA NO SEU FIND
sources = Glob('build/src/*.cpp') + \
          Glob('build/src/mappers/*.cpp') + \
          Glob('build/lib/*.cpp') + \
          Glob('build/lib/imgui/*.cpp') + \
          Glob('build/lib/imgui/backends/*.cpp')

# 3. Gerar o Executável
env.Program('laines', sources)
