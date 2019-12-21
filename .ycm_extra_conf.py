import subprocess
import re
import sys
import os.path

def Settings(**kwargs):
    # find directory of the file
    dirname = os.path.dirname(kwargs.get('filename', './'))

    os.chdir(dirname)
    make_run = subprocess.Popen(['make', '-s', 'flags'], stdout=subprocess.PIPE)
    flags = make_run.stdout.readline()
    make_run.kill()

    def expand_path(x):
        if x.startswith(('-I', '-L')):
            return x[:2] + os.path.abspath(x[2:])
        return x

    flags = [expand_path(f) for f in re.split(' +', flags.decode('utf-8').replace('\n', ''))]
    flags.extend(['-x', 'c++'])
    return {
        'flags': flags
    }

if __name__ == '__main__':
    print(Settings(filename=sys.argv[1]))
