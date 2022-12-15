import time

def ratelimit(once_every_millisec):

    prev_sec = time.monotonic()

    def deco(orig_fn): # how to name this func?

        def wrapper(*args, **kwargs):
            nonlocal prev_sec # nonlocal is required
            now_sec = time.monotonic()
            if((now_sec-prev_sec)*1000 >= once_every_millisec):
                prev_sec = now_sec
                return orig_fn(*args, **kwargs)
            pass

        return wrapper

    return deco


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    exit(0)

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
