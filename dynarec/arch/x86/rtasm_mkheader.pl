#! /usr/bin/perl

@lines = <>;

foreach (@lines)
{
  chomp;
  if (m/void/)
  {
    print "extern $_;\n";
  }
}
