#!/usr/bin/env python3
#coding: utf-8

# [2024-11-04] Major code by GPT4o

import os, sys
from PIL import Image, ImageDraw, ImageFont

mydir = os.path.dirname(__file__)

def genenrate_ico_file(letters, font_color):

	# Define the icon size and the letters to display
	icon_size = (32, 32)

	# Create a blank image with a transparent background
	icon = Image.new("RGBA", icon_size, (255, 255, 255, 0))
	draw = ImageDraw.Draw(icon)

	# Load a default font and adjust the size to fit the 32x32 icon
	try:
		# Using a default font available within PIL (as custom fonts might not be available)
		# Adjust font size as needed
		font = ImageFont.truetype(os.path.join(mydir, "Tahoma.ttf"), 24)
	except IOError:
		print("Bad! TTF Font loading fail.")
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
	filename = letters + ".ico"
	icon.save(filename, format="ICO", sizes=[icon_size])
	print("%s generated."%(filename))


if __name__=='__main__':
	arglen = len(sys.argv)
	if arglen<2:
		print("Usage: ")
		print('    letters-to-icon.py <letters> [color]')
		print("Example: ")
		print('    letters-to-icon.py "Hi" blue')
		print('    letters-to-icon.py "mg" #EE00FF')
		sys.exit(1)

	letters = sys.argv[1]
	color = sys.argv[2] if arglen>2 else "red"
	genenrate_ico_file(letters, color)

	sys.exit(0)
