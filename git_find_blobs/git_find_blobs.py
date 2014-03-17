#!/usr/bin/env python
import os
import sys
import subprocess

def human_readable_bytes(nbytes):
    suffixes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB']
    if nbytes == 0: return '0 B'
    i = 0
    while nbytes >= 1024 and i < len(suffixes)-1:
        nbytes /= 1024.
        i += 1
    f = ('%.2f' % nbytes)
    return '%s %s' % (f, suffixes[i])

def get_output(cmd, folder=None):
    if folder:
        folder = os.path.expanduser(folder)
    p = subprocess.Popen(cmd, cwd=folder, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    retval = p.wait()
    out = p.communicate()
    return retval, out[0], out[1]


def git_find_blobs(max_size, folder=None):
    retval, output, errors = get_output('git rev-list HEAD', folder)
    if retval:
        print errors
        return None

    max_padding = 0

    revisions = output.split()
    
    bigfiles = set()
    for revision in revisions:
        retval, output, errors = get_output('git ls-tree -zrl %s' % revision, folder)
        if retval:
            print errors
            return None

        files = output.split('\0')
        files = filter(None, files)
        for file in files:
            splitdata = file.split()
            commit = splitdata[2]
            if splitdata[3] == '-':
                continue

            size = int(splitdata[3])
            path = splitdata[4]
            if size > max_size:
                human_readable = human_readable_bytes(size)
                padding = len(human_readable)
                if padding > max_padding:
                    max_padding = padding

                bigfiles.add((size, human_readable, commit, path))

    bigfiles = sorted(bigfiles, reverse=True)

    for f in bigfiles:
        print f[1].rjust(max_padding), f[2], f[3] 

if __name__ == "__main__":
    folder = None
    if len(sys.argv) < 2:
        print "usage: %s size_in_bytes folder" % sys.argv[0]
        sys.exit(1)
    else:
        try:
            max_size = int(sys.argv[1])
        except ValueError:
            print "usage: %s size_in_bytes folder" % sys.argv[0]
            sys.exit(1)

        if len(sys.argv) > 2:
            folder = sys.argv[2]

    git_find_blobs(max_size, folder)
