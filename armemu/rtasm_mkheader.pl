#! /usr/bin/perl

$file = "rtasm_fns.c";
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
