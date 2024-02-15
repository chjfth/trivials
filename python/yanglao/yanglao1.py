#!/usr/bin/env python3
#coding: utf-8

import os, sys

prgname = os.path.basename(__file__)

years = 30        # 存款持续 30 年，每个月存一份养老金
year_rate_pct = 3.0 # 3% 年利率

def retire_monthly_income(years, year_rate_pct):
	assert(year_rate_pct>1 and year_rate_pct<=100)
	
	year_rate = 1 + year_rate_pct/100 # year_rate=1.05 etc
	month_rate = year_rate**(1/12)

	# 等比数列求和，共 years*12 项
	accum = (month_rate**(years*12) - 1) / (month_rate -1)

	return accum, accum * (month_rate-1)


def DengEBenXi(init_loan, month_rate, months):
	# 等额本息还款， 每月应还多少钱?
	assert(month_rate>1 and month_rate<=2)
	i = month_rate - 1
	fenzi = init_loan * i * month_rate**months
	fenmu = month_rate**months - 1
	return fenzi/fenmu

if __name__=='__main__':
	if len(sys.argv)<3:
		print("Usage:")
		print("    %s <years> <year_rate_pct>"%(prgname))
		print("Example:")
		print("    %s 30 3.0"%(prgname))
		exit(4)

	years = int(sys.argv[1])
	year_rate_pct = float(sys.argv[2])

	accum, monthly_income = retire_monthly_income(years, year_rate_pct)
	print("After retire,")
	print("    accumulated: %g"%(accum))
	print("    monthly interest income: %gX"%(monthly_income))

	# 假想 accum 这笔钱是我借给保险公司的，保险公司在未来 40 年向我等额本息还款，
	# 那么保险公司每个月应该还我多少钱？ 同时计算 40/35/30/25/20 还清的情况。
	year_rate = 1 + year_rate_pct/100 # year_rate=1.05 etc
	month_rate = year_rate**(1/12)
	for amyears in [40, 35, 30, 25, 20]:
		months = amyears * 12
		monthly_return = DengEBenXi(accum, month_rate, amyears*12)
		print("[AMY%d] monthly return %g"%(amyears, monthly_return))