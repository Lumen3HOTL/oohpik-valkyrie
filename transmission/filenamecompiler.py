import os
import sys
files=os.listdir()
extentions=[".py",".cpp",".txt",".h",".slnx",".vcxproj",".filters",".user",".wav",".mp4"]
for file in files:
    if(os.path.isfile(file)):
        
        if(os.path.splitext(file)[1] in extentions):
            print(file)

input()