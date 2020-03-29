testri means "test relative import".

This project demonstrate a way to deal with python's mystical "attempted relative import beyond top-level package" error message.

In short, just prepare an extra level of file-system parent directory, and use that directory as "top-level package", and the mystical error is gone.

```
$ python testfoo1.py
dirtoppkg = /home/chj/gitw/trivials/python/testri
Traceback (most recent call last):
  File "testfoo1.py", line 7, in <module>
    from foo import hellofoo
  File "/home/chj/gitw/trivials/python/testri/foo/hellofoo.py", line 1, in <module>
    from ..bar import hellobar
ValueError: Attempted relative import beyond toplevel package
```

```
$ python testfoo2.py
dirtoppkg = /home/chj/gitw/trivials/python
hellofoo.
hellobar.

```

As you can see above, just a difference in `dirtoppkg` (at outside absolute-import "anchor point") makes relative-import work smoothly inside.
