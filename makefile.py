import os
import subprocess
import sys

# Only change the following three lines
makefile = 'src/Makefile'
exec = 'src/main'
arguments = ''

client_file = 'D:\Dev\Projects\Graphics_Rendering_Tool\client\graphics_client.py'

if len(sys.argv) > 2:
    arguments = ' '.join(sys.argv[2:])

subprocess.call(' '.join(('py -3', client_file, sys.argv[1], os.getcwd(), makefile, exec, arguments)), shell=True)