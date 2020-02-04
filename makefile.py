import os
import subprocess
import sys

# Only change the following three lines
makefile = 'project4/Makefile'
exec = 'project4/main'
arguments = ''

client_file = '~/dev/Graphics_Rendering_Tool/client/graphics_client.py'

if len(sys.argv) > 2:
    arguments = ' '.join(sys.argv[2:])

subprocess.call(' '.join(('python3', client_file, sys.argv[1], os.getcwd(), makefile, exec, arguments)), shell=True)