[2019-09-20]
This Eclipse project shows that "eclisub/src" and "eclisub/test" should be set as <classpathentry>(so-called "Source Folder" in Eclipse IDE, same meaning as env-var CLASSPATH).

The wrong way is to set "eclisub" as "Source Folder", because the "package xxx" statement at start of a .java file determines what directory should be put into CLASSPATH.

Normally, in Eclipse IDE, on scanning an existing base-folder for new java project creation, Eclipse will discover the fact and assign correct "Source Folders" for us.
