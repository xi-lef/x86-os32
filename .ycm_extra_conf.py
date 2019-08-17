def Settings( **kwargs ):
  return {
    'flags': [ '-x', 'c++', '-std=c++11', '-m32', '-Wall', '-I', '/opt/Dropbox/universitaet/semester5/bs/mpstubs', 
        '-Wextra', '-ffreestanding', '-fno-builtin', '-nodefaultlibs', '-nostdlib', '-nostdinc', '-Wno-write-strings', '-fno-stack-protector' ],
  }
