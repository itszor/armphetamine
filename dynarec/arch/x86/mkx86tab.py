#!/usr/bin/env python

import sys

from sre import *
from string import *

def expandflags(pat):
  xpat = []
  if find(pat, 'c') != -1:
    xpat.append('ph_C')
  if find(pat, 'v') != -1:
    xpat.append('ph_V')
  if find(pat, 'n') != -1:
    xpat.append('ph_N')
  if find(pat, 'z') != -1:
    xpat.append('ph_Z')
  if find(pat, 'i') != -1:
    xpat.append('ph_IC')

  if xpat == []: xpat = ['0']
  
  return xpat

infile=sys.stdin
# open('genx86tabsrc.dat', 'r')
outfile=sys.stdout
# open('genx86_tab.c', 'w')

lines = infile.readlines()

splitre = compile(r'\|')
comma = compile(r',')

things = {'m':   (0, '_rm8'),
          'i':   (1, '_imm8'),
          'o':   (1, '_rel8'),
          'rm':  (2, '_r8_rm8'),
          'mr':  (3, "_rm8_r8"),
          'ri':  (4, "_r8_imm8"),
          'mi':  (5, "_rm8_imm8"),
          'M':   (6, "_rm32"),
          'I':   (7, "_imm32"),
          'O':   (7, "_rel32"),
          'R':   (8, "_r32"),
          'RM':  (9, "_r32_rm32"),
          'RMs': (9, "_r32_m32"),
          'MR':  (10, "_rm32_r32"),
          'RI':  (11, "_r32_imm32"),
          'Mi':  (12, "_rm32_imm8"),
          'MI':  (13, "_rm32_imm32"),
          'Mc':  (14, "_rm32_cl"),
          'RMi': (15, "_r32_rm32_imm8"),
          'RMI': (16, "_r32_rm32_imm32"),
          'n':   (17, ""),
         }

outfile.write('#include "rtasm.h"\n')
outfile.write('#include "genx86.h"\n\n')
outfile.write('const genx86_variant genx86_tab[] =\n{\n')

array = []

for line in lines:
  (op,variant,set,corrupt) = splitre.split(sub(r'\s*', '', line))
  wotsit = comma.split(variant)
  output = ['NULL'] * 18

  for bob in wotsit:
    (offset,postfix) = things[bob]
    output[offset] = 'rtasm_' + op + postfix

  seta = expandflags(set)
  corrupta = expandflags(corrupt)

  array.append('  {\t' + join(output, ',\n\t') + ',\n\t' +
    join(seta, '|') + ', ' + join(corrupta, '|') + ' }')

outfile.write(join(array, ',\n') + '\n};\n\n')


