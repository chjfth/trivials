#!/usr/bin/env python3
#coding: utf-8

import os, sys

progname = os.path.basename(__file__)

if len(sys.argv)==1:
	print("Replace matching multi-line text with new text in a folder.")
	print("Example:")
	print("  %s .vcxproj"%(progname))
	print("  %s .vcxproj replace"%(progname))
	print("")
	print("NOTE: Scan folder, source text and replacement text are defined in %s source code."%(progname))
	sys.exit(1)

DOT_EXTNAME = sys.argv[1]
DO_REPLACE = False
if len(sys.argv)>2 and sys.argv[2]=="replace":
	DO_REPLACE = True

rootdir = r'D:\gitw\bookcode-mswin\PRWIN5'

oldlines = r"""  <!-- import VSPG start -->
  <PropertyGroup>
    <VSPG_BatDir_NoTBS>..\..\_VSPG</VSPG_BatDir_NoTBS>
    <VSPG_FeedbackFile>$(RootNamespace).cpp</VSPG_FeedbackFile>
  </PropertyGroup>
  <ImportGroup Label="PropertySheets">
    <Import Project="$(VSPG_BatDir_NoTBS)\_VSPG.props" />
  </ImportGroup>
  <!-- import VSPG end -->
"""

newlines = r"""  <!-- import VSPG start -->
  <PropertyGroup>
    <VSPG_BootsDir_NoTBS>..\..\..\_VSPG\boots</VSPG_BootsDir_NoTBS>
  </PropertyGroup>
  <ImportGroup Label="PropertySheets">
    <Import Project="$(VSPG_BootsDir_NoTBS)\_VSPG.props" />
  </ImportGroup>
  <!-- import VSPG end -->
"""

count = 0

for root,dirs,files in os.walk(rootdir):
	for file in files:
		if not file.endswith(DOT_EXTNAME):
			continue
		
		filepath = os.path.join(root, file)
		
		with open(filepath, "r+") as fh:
			oldtext = fh.read()
			
			newtext = oldtext.replace(oldlines, newlines)
			
			if newtext!=oldtext:
				
				count += 1
				
				if DO_REPLACE:
					print("[REPLACE] %s"%(os.path.join(root, file)))
					
					fh.seek(0)
					fh.write(newtext)
					fh.truncate()
				else:
					print("[FOUND] %s"%(os.path.join(root, file)))
		

print("")
print("Scanned folder: %s"%(rootdir))
print("Source text:")
print(oldlines)

if DO_REPLACE:
	print("Replacement text:")
	print(newlines)
	print("Replaced in %d files."%(count))
else:
	print("Found in %d files."%(count))

