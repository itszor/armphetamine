#! /usr/bin/perl

$file = "/dev/stdin";
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
