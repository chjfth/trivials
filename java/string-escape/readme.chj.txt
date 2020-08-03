[2020-08-03]
This folder shows a puzzle for java beginners.
In Teacher.java, there is an unfinished part in the code, denoted by ??? below:

	System.out.println(\"???\");\n

What should we substitute the ??? for, so to achieve such a result:
When Teacher.class is run, it outputs content for Student.java, 
then we compile and run Student.java, its output exactly matches homework0.txt .

ANSWER BELOW
============

Required libraries:
https://commons.apache.org/proper/commons-text/
	https://commons.apache.org/proper/commons-lang/download_lang.cgi

Command line:

javac -classpath ../commons-lang3-3.11.jar;../commons-text-1.9.jar Teach.java
java -classpath ../commons-lang3-3.11.jar;../commons-text-1.9.jar;. Teach

Run output:

He\\\"ll\\\"o\\r\\n\\\\World/


To compile and run Teacher & Student:

javac Teacher.java
java Teacher > Student.java
javac Student.java
java Student > homework.txt

The homework0.txt and homework.txt should be exactly the same.
