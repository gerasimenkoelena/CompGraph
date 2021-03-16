import os

indir = os.getcwd()
mask = set()
off = open('mask.txt', 'w')
for root, dirs, filenames in os.walk(indir):
	for f in filenames:
		tmp = len(mask)
		mask.add(f[f.rfind('.'):])
		if(len(mask) != tmp):
			print(root + '\\' + f)
			off.write("'"+f[f.rfind('.'):]+ "'"+', ')
