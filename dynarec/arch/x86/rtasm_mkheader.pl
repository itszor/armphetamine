#! /usr/bin/perl

$file = "dynarec/arch/x86/rtasm_fns.c";
open(file);
@lines = <file>;
close(file);

foreach (@lines)
{
  chop;
  if (m/void/)
  {
    print "extern $_;\n";
  }
}
