#!/usr/bin/perl

$input = "/home/jules/ARMemu/inteldocs/instructions.txt";

open(input);
@def = <input>;
close(input);

$deflen = @def;

$index = 0;
while ($index<=$deflen)
{
  $_ = @def[$index++];
  if (m/Opcode\s*Instruction\s*Description/)
  {
    $_ = @def[$index++];
    while (!m/^\s*$/)  # non-blank lines
    {
#      s/\s+/ /g;  # remove duplicate spaces
      s/^\s//g;   # remove space from start of line
#      @parts = split(/\s/);
#      $numparts = @parts;
#      for ($part = 0; $part<=$numparts; $part++)
#      {
#        $this = @parts[$part];
#        if ($this =~ m/^\s*[0-9A-F][0-9A-F](![A-F])/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/\/r/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/\/[0-9]/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/ib/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/iw/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/id/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/cb/) {
#          print "$this ";
#        }
#        elsif ($this =~ m/cw\/cd/) {
#          print "$this ";
#        }
#        else {
#          last;
#        }
#      }
#      print "@parts[$part]\n";
      print();
      $_ = @def[$index++];
    }
  }
}
