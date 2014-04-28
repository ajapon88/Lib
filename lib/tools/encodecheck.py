# -*- coding: utf-8 -*-

import sys
import re
from os import path
	
if __name__ == '__main__':
	argv = sys.argv
	argc = len(argv)
	if (argc != 4):
		print 'usage: %s encode newline_code filename' % path.basename(__file__)
		quit()
		
	encode = argv[1]
	check_encode = encode
	newline_code = argv[2]
	filename = argv[3]
	bomcheck = False
	isbom = False
	
	if (encode == 'utf-8'):
		bomcheck = True
		isbom = False
	if (encode == 'utf-8-bom'):
		bomcheck = True
		isbom = True
		check_encode = 'utf-8'
	
	try:
		str = open(filename,'rb').read()
	except:
		print 'file open error "%s"' % filename
		sys.exit(1)
	
	if (bomcheck):
		if (re.match('^\xEF\xBB\xBF', str)):
			if (not isbom):
				print 'file "%s" is not encode %s' % (filename, encode)
				sys.exit(1)
			str=re.sub('^\xEF\xBB\xBF', '', str)
		elif (isbom):
			print 'file "%s" is not encode %s' % (filename, encode)
			sys.exit(1)
	try:
		str.decode(check_encode)
	except:
		print 'file "%s" is not encode %s' % (filename, encode)
		sys.exit(1)

	if (newline_code == 'CR'):
		incorrect_nl_pattern = '(\r)|(\r\n)'
	elif (newline_code == 'LF'):
		incorrect_nl_pattern = '(\n)|(\r\n)'
	elif (newline_code == 'CRLF'):
		incorrect_nl_pattern = '([^\r]\n)|(\r[^\n])'
	else:
		print 'wrong newline code %s' % newline_code
		sys.exit(1)

	if (re.match(incorrect_nl_pattern, str)):
		print 'file "%s" is not newline code %s' % (filename, newline_code)
		sys.exit(1)