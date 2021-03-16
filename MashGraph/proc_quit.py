import os
import re
import shutil
print()
indir = os.getcwd()
for root, dirs, filenames in os.walk(indir):
    if re.search(r'_old', root):
        shutil.rmtree(root) 

mod_list = next(os.walk(indir))
src_name = ''.join(mod_list[1])
path = ''.join(mod_list[0]) + '' + src_name
log = open('log.txt', 'w')
masks = ['.sln', '.txt', '.py', '.suo', '.db', '.ipch', '.exe', '.ilk', '.pdb', '.h', '.editorconfig', '.inl', '.in', '.hpp', '.cpp', '.lib', '.png', '.jpg', '.vcxproj', '.filters', '.user', '.obj', '.log', '.idb', '.tlog', '.lastbuildstate', '.blend', '.mtl', '.shader', '.tga']


for root, dirs, filenames in os.walk(indir):
    if re.search(r'_old', root) or root == os.getcwd():
        continue
    #print('ROOT', root)
    for f in filenames:
        off = open(root + '\\' + f, 'r', encoding="ISO-8859-1")
    for f in filenames:
        if f[f.rfind('.'):] in masks and os.path.isdir(path + '\\' + f) == False:
            off = open(root + '\\' + f, 'r', encoding="ISO-8859-1")
            comment, num_line, flag, proc_sql = [0,1,0,0]
            for line in off:
                if re.match(r'.*c:\\.*\\mashgraph', line.lower()):
                    print(root + '\\' + f)
                    input()
                    break
                