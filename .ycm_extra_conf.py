import subprocess
import re
import sys
import os.path

def FlagsForFile( filename, **kwargs ):
    # find directory of the file
    dirname = os.path.dirname(filename)

    os.chdir(dirname)
    make_run = subprocess.Popen(['make', '-s', 'flags'], stdout=subprocess.PIPE)
    flags = make_run.stdout.readline()
    make_run.kill()
    
    def expand_path(x):
        if x.startswith(('-I', '-L')):
            return x[:2] + os.path.abspath(x[2:])
        return x

    flags = map(expand_path, re.split(' +', flags.replace('\n', '')))
    flags.extend(['-x', 'c++'])
    return {
        'flags': flags,
        'do_cache': True
    }

if __name__ == '__main__':
   print(FlagsForFile(sys.argv[1]))
