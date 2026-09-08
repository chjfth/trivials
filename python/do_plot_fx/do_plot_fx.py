#!/usr/bin/env python3
#coding: utf-8

# Need python 3.7+
# To display wide-range Unicode characters, see my Evclip 20260110.c1 .

import math
import matplotlib
import matplotlib.pyplot as plt

def Enable_chs_font(fontname='Microsoft YaHei'):
	# Thanks https://my.oschina.net/u/1180306/blog/279818
	matplotlib.rcParams['font.sans-serif'] = fontname

def is_iter(obj):
	try:
		it = iter(obj)
		return True
	except TypeError:
		return False

def do_plot_fx(fx, xstart, xend, nsteps, ylimit=None, fr=False, draw_axis=None,
	xlabel='', ylabel=''):
   
	if fr: # fixed aspect ratio, a circle looks like a circle
		plt.axis('equal')
   
	if draw_axis:
		plt.axhline(y=draw_axis[1], color='grey')
		plt.axvline(x=draw_axis[0], color='grey')

	fx_list = fx if is_iter(fx) else [fx]
   
	if is_iter(ylimit):
		ylimits = (ylimit[0], ylimit[1])
	elif ylimit:
		ylimits = (-ylimit, ylimit)
	else:
		ylimits = None

	xrange = xend - xstart
	xstep = xrange / nsteps
	x_list = [ xstart+xstep*i for i in range(nsteps+1) ]
   
	for fx in fx_list:
		# Each function from user:
		y_list = []
		for i, x in enumerate(x_list):
			# Each point:
			try:
				y = fx(x)
				if ylimits and (y<ylimits[0] or y>ylimits[1]):
					y = math.nan
			except (ZeroDivisionError, OverflowError, ValueError):
				y = math.nan
	   
			y_list.append(y)
		   
		plt.plot(x_list, y_list, label=getattr(fx,'label',None), color=getattr(fx,'color',None))
	   
	plt.legend() # to show user-given label
	plt.grid(linestyle='dotted')
   
	plt.xlabel(xlabel)
	plt.ylabel(ylabel)
   
	Enable_chs_font()
	plt.show()


if __name__=='__main__':
	def sinx(x): return math.sin(x)
	def sinxx(x): return math.sin(x)/x 
	
	sinx.label='正弦波' # optional label
	sinxx.label='sin(x)/x' 
	
	do_plot_fx((sinx, sinxx), -5*math.pi, 5*math.pi, 500, draw_axis=(0,0))

