#! /usr/bin/env python

import re
import os 
import string
import sys

inputfile = open(sys.argv[1],'r')
doc = inputfile.read()
doc = doc.replace(".START","")
print doc
