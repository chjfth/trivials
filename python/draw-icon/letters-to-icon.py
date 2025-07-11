#!/usr/bin/env python3
#coding: utf-8

# [2024-11-04] Major code by GPT4o

import os, sys
from PIL import Image, ImageDraw, ImageFont

mydir = os.path.dirname(__file__)

def genenrate_ico_file(letters, font_color, fontfile):

	# Define the icon size and the letters to display
	icon_size = (32, 32)

	# Create a blank image with a transparent background
	icon = Image.new("RGBA", icon_size, (255, 255, 255, 0))
	draw = ImageDraw.Draw(icon)

	# Load a default font and adjust the size to fit the 32x32 icon
	try:
		# Using a default font available within PIL (as custom fonts might not be available)
		# Adjust font size as needed
		font = ImageFont.truetype(os.path.join(mydir, fontfile), 24)
	except IOError:
		print("Bad! TTF Font loading fail:")
		print("    " + FONTFILE)
		sys.exit(1) # font = ImageFont.load_default()

	# Calculate text position to center it in the icon using textbbox
	bbox = draw.textbbox((0, 0), letters, font=font) # For "AB", return (-1,7, 18,25)
	ybias = bbox[1]

	text_width, text_height = bbox[2] - bbox[0], bbox[3] - bbox[1]
	text_position = [ (icon_size[0]-text_width)//2, (icon_size[1]-text_height)//2 ]

	text_position[1] -= ybias # chj: should remove the top-margin blank area(called internal-leading?)

	# Draw the letters on the icon
	draw.text(text_position, letters, font=font, fill=font_color)

	# Save the image as an .ico file
	# Note that this have 32bit-png as .ico's internal format, which is not friendly with WinXP.
	# Embedding such .ico into an EXE, WinXP will not show that EXE' icon in Explorer.
	filename = letters + ".ico"
	icon.save(filename, format="ICO", sizes=[icon_size])
	print("%s generated."%(filename))

	# Save a .png as well, so that we can later Ubuntu `icotool` to make a BMP-style ico.
	# 	sudo apt install icoutils
	# 	icotool -c  AB.ico.png  -o AB-bmp.ico 
	#
	filename_png = filename + ".png"
	icon.save(filename_png, format="PNG", sizes=[icon_size])
	print("%s generated."%(filename_png))
	
	print("")
	print("Hint: To make the exe-embedded icon recognized by WinXP, ")
	print("      you need to further convert it to be a BMP-backed ico,")
	print("      using command:")
	print("icotool -c  %s  -o %s-bmp.ico"%(filename_png, letters))


if __name__=='__main__':
	arglen = len(sys.argv)
	if arglen<2:
		print("Usage: ")
		print('    letters-to-icon.py <letters> [color]')
		print("Example: ")
		print('    letters-to-icon.py "OK" blue')
		print('    letters-to-icon.py "Pg1" "#EE00FF"') 
		print('Env-var:')
		print('    FONTFILE : This assigns a ttf file to use as drawing font.')
		# -- Need to enclose #RGB in quotes, bcz Linux shell considers # as comment start.
		sys.exit(1)

	letters = sys.argv[1]
	color = sys.argv[2] if arglen>2 else "red"
	
	fontfile = os.getenv('FONTFILE')
	if not fontfile:
		nchars = len(letters)
		if nchars<=3:
			fontfile = 'BigShouldersText-Bold.ttf'
		elif nchars==4:
			fontfile = 'MouseMemoirs-Regular.ttf'
		else:
			fontfile = 'LeagueGothic_Condensed-Regular.ttf'

	genenrate_ico_file(letters, color, fontfile)

	sys.exit(0)
