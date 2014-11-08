import subprocess
import re
import sys

def FlagsForFile( filename, **kwargs ):
    make_run = subprocess.Popen(['make', 'flags'], stdout=subprocess.PIPE)
    flags = make_run.stdout.readline()
    make_run.kill()
    flags = re.split(' +', flags.replace('\n', ''))
    flags.extend(['-x', 'c++'])
    return {
        'flags': flags,
        'do_cache': True
    }

if __name__ == '__main__':
   print(FlagsForFile(sys.argv[1]))
