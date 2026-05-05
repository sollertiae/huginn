import os, errno
try:
    fd = os.open('/proc/sleep', os.O_RDONLY | os.O_NONBLOCK)
    print(fd)
except OSError as e:
    print(f'Error {e}')
