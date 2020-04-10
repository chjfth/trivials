[2020-04-09]

Detail experiment memo at: https://www.evernote.com/l/ABXJkgsV0tFDQKoVppWeoco6DqKJf_VuqHI/

====================
Console output below
====================


(env37a) d:\gitw\trivials\python\import3v>ipython
Python 3.7.4 (tags/v3.7.4:e09359112e, Jul  8 2019, 20:34:20) [MSC v.1916 64 bit (AMD64)]
Type 'copyright', 'credits' or 'license' for more information
IPython 7.13.0 -- An enhanced Interactive Python. Type '?' for help.

In [1]: import pkgx
[d:\gitw\trivials\python\import3v\pkgx\__init__.py] __name__=pkgx
[d:\gitw\trivials\python\import3v\pkgx\sub3.py] __name__=pkgx.sub3

In [2]: dir(pkgx)
Out[2]:
['__builtins__',
 '__cached__',
 '__doc__',
 '__file__',
 '__loader__',
 '__name__',
 '__package__',
 '__path__',
 '__spec__',
 'sub2',
 'sub3']

In [3]: pkgx.sub1
---------------------------------------------------------------------------
AttributeError                            Traceback (most recent call last)
<ipython-input-3-08680f806416> in <module>
----> 1 pkgx.sub1

AttributeError: module 'pkgx' has no attribute 'sub1'

In [4]: pkgx.sub2
Out[4]: 'sub2 is string'

In [5]: pkgx.sub3
Out[5]: <module 'pkgx.sub3' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\sub3.py'>

In [6]: exit()

(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>ipython
Python 3.7.4 (tags/v3.7.4:e09359112e, Jul  8 2019, 20:34:20) [MSC v.1916 64 bit (AMD64)]
Type 'copyright', 'credits' or 'license' for more information
IPython 7.13.0 -- An enhanced Interactive Python. Type '?' for help.

In [1]: import pkgx.sub1
[d:\gitw\trivials\python\import3v\pkgx\__init__.py] __name__=pkgx
[d:\gitw\trivials\python\import3v\pkgx\sub3.py] __name__=pkgx.sub3
[d:\gitw\trivials\python\import3v\pkgx\sub1.py] __name__=pkgx.sub1

In [2]: dir(pkgx)
Out[2]:
['__builtins__',
 '__cached__',
 '__doc__',
 '__file__',
 '__loader__',
 '__name__',
 '__package__',
 '__path__',
 '__spec__',
 'sub1',
 'sub2',
 'sub3']

In [3]: pkgx.sub1
Out[3]: <module 'pkgx.sub1' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\sub1.py'>

In [4]: pkgx.sub2
Out[4]: 'sub2 is string'

In [5]: pkgx.sub3
Out[5]: <module 'pkgx.sub3' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\sub3.py'>

In [6]: pkgx
Out[6]: <module 'pkgx' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\__init__.py'>

In [7]: exit()

(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>ipython
Python 3.7.4 (tags/v3.7.4:e09359112e, Jul  8 2019, 20:34:20) [MSC v.1916 64 bit (AMD64)]
Type 'copyright', 'credits' or 'license' for more information
IPython 7.13.0 -- An enhanced Interactive Python. Type '?' for help.

In [1]: import pkgx.sub2
[d:\gitw\trivials\python\import3v\pkgx\__init__.py] __name__=pkgx
[d:\gitw\trivials\python\import3v\pkgx\sub3.py] __name__=pkgx.sub3
[d:\gitw\trivials\python\import3v\pkgx\sub2.py] __name__=pkgx.sub2

In [2]: dir(pkgx)
Out[2]:
['__builtins__',
 '__cached__',
 '__doc__',
 '__file__',
 '__loader__',
 '__name__',
 '__package__',
 '__path__',
 '__spec__',
 'sub2',
 'sub3']

In [3]: pkgx.sub1
---------------------------------------------------------------------------
AttributeError                            Traceback (most recent call last)
<ipython-input-3-08680f806416> in <module>
----> 1 pkgx.sub1

AttributeError: module 'pkgx' has no attribute 'sub1'

In [4]: pkgx.sub2
Out[4]: <module 'pkgx.sub2' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\sub2.py'>

In [5]: pkgx.sub3
Out[5]: <module 'pkgx.sub3' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\sub3.py'>

In [6]: exit()

(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>
(env37a) d:\gitw\trivials\python\import3v>ipython
Python 3.7.4 (tags/v3.7.4:e09359112e, Jul  8 2019, 20:34:20) [MSC v.1916 64 bit (AMD64)]
Type 'copyright', 'credits' or 'license' for more information
IPython 7.13.0 -- An enhanced Interactive Python. Type '?' for help.

In [1]: import pkgx.sub3
[d:\gitw\trivials\python\import3v\pkgx\__init__.py] __name__=pkgx
[d:\gitw\trivials\python\import3v\pkgx\sub3.py] __name__=pkgx.sub3

In [2]: dir(pkgx)
Out[2]:
['__builtins__',
 '__cached__',
 '__doc__',
 '__file__',
 '__loader__',
 '__name__',
 '__package__',
 '__path__',
 '__spec__',
 'sub2',
 'sub3']

In [3]: pkgx.sub1
---------------------------------------------------------------------------
AttributeError                            Traceback (most recent call last)
<ipython-input-3-08680f806416> in <module>
----> 1 pkgx.sub1

AttributeError: module 'pkgx' has no attribute 'sub1'

In [4]: pkgx.sub2
Out[4]: 'sub2 is string'

In [5]: pkgx.sub3
Out[5]: <module 'pkgx.sub3' from 'd:\\gitw\\trivials\\python\\import3v\\pkgx\\sub3.py'>

In [6]: exit()