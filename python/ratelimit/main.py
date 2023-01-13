import time
from datetime import datetime

from ratelimit import ratelimit

g_count = 0

# The decorator @ratelimit(500), will limit actual calling rate of
# print_now_time() to be at most once every 500ms.

@ratelimit(500)
def print_now_time(prefix):
    global g_count
    g_count += 1
    dt = datetime.now()
    print("%s[%2d] Now %02d:%02d:%02d.%06d"%(prefix, g_count,
        dt.hour, dt.minute, dt.second, dt.microsecond))

def test():
    for i in range(19):
        print_now_time('+' if i%2 else '-')
        time.sleep(0.1)

    print_now_time('*')

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    test()
    exit(0)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
