#!/usr/bin/perl -w

@iname = ("and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
          "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn");

%islogic = ("and", 1, "eor", 1, "sub", 0, "rsb", 0,
            "add", 0, "adc", 0, "sbc", 0, "rsc", 0,
            "tst", 1, "teq", 1, "cmp", 0, "cmn", 0,
            "orr", 1, "mov", 1, "bic", 1, "mvn", 1);

@sname = ("lsl", "lsr", "asr", "ror");

print ".text\n";

foreach $inst (@iname)
{
  foreach $setflags ('s', 'ns')
  {
    foreach $shifttype (@sname)
    {
      foreach $regshift ('reg', 'imm')
      {
        $funcname = "${inst}_${setflags}_${shifttype}_${regshift}";
        print ".p2align 4,,7\n";
        print ".globl $funcname\n";
        print ".type $funcname,\@function\n";
        print "$funcname:\n";
        
        outputShift();
        
        print "\tmovl 4(\%esp),\%eax\n";
        
        $invc = 0;
        $needtest = 0;
        
        if ($inst eq 'and') {
          print "\tandl \%edx,\%eax\n";
        }
        elsif ($inst eq 'eor') {
          print "\txorl \%edx,\%eax\n";
        }
        elsif ($inst eq 'sub') {
          print "\tsubl \%edx,\%eax\n";
          $invc = 1;
        }
        elsif ($inst eq 'rsb') {
          print "\txchgl \%edx,\%eax\n";
          print "\tsubl \%edx,\%eax\n";
        }
        elsif ($inst eq 'add') {
          print "\taddl \%edx,\%eax\n";
        }
        elsif ($inst eq 'adc') {
          print "\tmovl 16(\%esp),\%ecx\n";
          print "\tbt \$29,(\%ecx)\n";
          print "\tadcl \%edx,\%eax\n";
        }
        elsif ($inst eq 'sbc') {
          print "\tmovl 16(\%esp),\%ecx\n";
          print "\tbt \$29,(\%ecx)\n";
          print "\tcmc\n";
          print "\tsbbl \%edx,\%eax\n";
          $invc = 1;
        }
        elsif ($inst eq 'rsc') {
          print "\tmovl 16(\%esp),\%ecx\n";
          print "\txchgl \%edx,\%eax\n";
          print "\tbt \$29,(\%ecx)\n";
          print "\tcmc\n";
          print "\tsbbl \%edx,\%eax\n";
          $invc = 1;
        }
        elsif ($inst eq 'tst') {
          print "\ttestl \%edx,\%eax\n";
        }
        elsif ($inst eq 'teq') {
          print "\tmovl \%eax,\%ecx\n";
          print "\txorl \%edx,\%ecx\n";
        }
        elsif ($inst eq 'cmp') {
          print "\tcmpl \%edx,\%eax\n";
          $invc = 1;
        }
        elsif ($inst eq 'cmn') {
          print "\tmovl \%eax,\%ecx\n";
          print "\taddl \%edx,\%ecx\n";
        }
        elsif ($inst eq 'orr') {
          print "\torl \%edx,\%eax\n";
        }
        elsif ($inst eq 'mov') {
          print "\tmovl \%edx,\%eax\n";
          $needtest = 1;
        }
        elsif ($inst eq 'bic') {
          print "\tnotl \%edx\n";
          print "\tandl \%edx,\%eax\n";
        }
        elsif ($inst eq 'mvn') {
          print "\tnotl \%edx\n";
          print "\tmovl \%edx,\%eax\n";
          $needtest = 1;
        }
        
        if ($setflags eq 's')
        {
          if ($islogic{$inst})
          {
            if ($needtest) {
              print "\ttestl \%eax,\%eax\n";
            }
            print "\tsete \%dl\n";
            print "\tsets \%dh\n";
            print "\tshl \$1,\%dh\n";
            print "\tor \%dh,\%dl\n";
            print "\tshl \$30,\%edx\n";
            print "\tmovl 16(\%esp),\%ecx\n";
            print "\tandl \$0x3fffffff,(\%ecx)\n";
            print "\torl \%edx,(\%ecx)\n";
          }
          else  # arithmetic instruction
          {
            print "\tseto \%dl\n";     # V
            if ($invc) {
              print "\tsetnc \%dh\n";  # !C
            }
            else {
              print "\tsetc \%dh\n";   # C
            }
            print "\tsete \%cl\n";     # Z
            print "\tsets \%ch\n";     # N
            print "\tshlb \$1,\%dh\n";
            print "\tshlb \$2,\%cl\n";
            print "\tshlb \$3,\%ch\n";
            print "\torb \%dh,\%dl\n";
            print "\torb \%cl,\%dl\n";
            print "\torb \%ch,\%dl\n";
            print "\tshll \$28,\%edx\n";
            print "\tmovl 16(\%esp),\%ecx\n";
            print "\tandl \$0x0fffffff,(\%ecx)\n";
            print "\torl \%edx,(\%ecx)\n";
          }
        }
        print "\tret\n\n";
      }
    }
  }
}

sub outputShift
{
  my ($flags) = $setflags eq 's' && $islogic{$inst};

  my $label = sub {
    return ($funcname . '_' . $_[0] . ":\n");
  };

  my $ref = sub {
    return ($funcname . '_' . $_[0] . "\n");
  };

  if ($regshift eq 'reg')
  {
    if ($shifttype eq 'lsl')
    {
      print "\tmovzbl 12(\%esp),\%ecx\n";
      print "\tmovl 8(\%esp),\%edx\n";
      print "\tcmp \$31,\%cl\n";
      print "\tjg " . &$ref(1);
      print "\tshll \%cl,\%edx\n";
      if ($flags)
      {
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
      print "\tjmp " . &$ref(2);
      print &$label(1);
      print "\tcmp \$32,\%cl\n";
      print "\tje " . &$ref(3);
      # greater than 32-bit shift
      if ($flags)
      {
        print "\tbtr \$29,(\%eax)\n"; # clear carry
      }
      print "\txorl \%edx,\%edx\n";   # clear edx
      print "\tjmp " . &$ref(2);
      print &$label(3);
      if ($flags)
      {
        print "\tbt \$31,\%edx\n";
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
      print "\txorl \%edx,\%edx\n";
      print &$label(2);
    }
    elsif ($shifttype eq 'lsr')
    {
      print "\tmovzbl 12(\%esp),\%ecx\n";
      print "\tmovl 8(\%esp),\%edx\n";
      print "\tcmp \$31,\%cl\n";
      print "\tjg " . &$ref(1);
      print "\tshrl \%cl,\%edx\n";
      if ($flags)
      {
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
      print "\tjmp " . &$ref(2);
      print &$label(1);
      print "\tcmp \$32,\%cl\n";
      print "\tje " . &$ref(3);
      # greater than 32-bit shift
      if ($flags)
      {
        print "\tbtr \$29,(\%eax)\n"; # clear carry
      }
      print "\txorl \%edx,\%edx\n";   # clear edx
      print "\tjmp " . &$ref(2);
      print &$label(3);
      if ($flags)
      {
        print "\tbt \$0,\%edx\n";
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
      print "\txorl \%edx,\%edx\n";
      print &$label(2);    
    }
    elsif ($shifttype eq 'asr')
    {
      print "\tmovzbl 12(\%esp),\%ecx\n";
      print "\tmovl 8(\%esp),\%edx\n";
      print "\tcmp \$31,\%cl\n";
      print "\tjg " . &$ref(1);
      print "\tsarl \%cl,\%edx\n";
      if ($flags)
      {
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
      print "\tjmp " . &$ref(2);
      print &$label(1);
      # greater than 31-bit shift
      print "\tbt \$31,\%edx\n";
      print "\txorl \%edx,\%edx\n";  # clear edx
      print "\tsbbl \$0,\%edx\n";
      if ($flags)
      {
        print "\tmovl \%edx,\%eax\n";
        print "\tandl \$0x20000000,\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%eax,(\%eax)\n";
      }
      print &$label(2);
    }
    elsif ($shifttype eq 'ror')
    {
      print "\tmovzbl 12(\%esp),\%ecx\n";
      print "\tmovl 8(\%esp),\%edx\n";
      # register-valued ROR of zero doesn't seem to affect C
      if ($flags)
      {
        print "\tcmp \$0,\%cl\n";
        print "\tje " . &$ref(1);
      }
      print "\trorl \%cl,\%edx\n";
      if ($flags)
      {
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
        print &$label(1);
      }
    }
  }
  else  # immediate shift
  {
    if ($shifttype eq 'lsl')
    {
      print "\tmovl 12(\%esp),\%ecx\n";
      print "\tmovl 8(\%esp),\%edx\n";
      print "\tshll \%cl,\%edx\n";
      if ($flags)
      {
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
    }
    elsif ($shifttype eq 'lsr')
    {
      if ($flags)
      {
        print "\tmovl 12(\%esp),\%ecx\n";
        print "\tcmpl \$0,\%ecx\n";
        print "\tje " . &$ref(1);
        print "\tmovl 8(\%esp),\%edx\n";
        print "\tshrl \%cl,\%edx\n";
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
        print "\tjmp " . &$ref(2);
        print &$label(1);
        print "\tbt \$31,\%edx\n";
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
        print &$label(2);
      }
      else
      {
        print "\tmovl 12(\%esp),\%ecx\n";
        print "\tcmpl \$0,\%ecx\n";
        print "\tje " . &$ref(1);
        print "\tmovl 8(\%esp),\%edx\n";
        print "\tshrl \%cl,\%edx\n";
        print "\tjmp " . &$ref(2);
        print &$label(1);
        print "\tmovl 4(\%esp),\%eax\n";
        print "\tret\n";
        print &$label(2);
      }
    }
    elsif ($shifttype eq 'asr')
    {
      if ($flags)
      {
        print "\tmovl 12(\%esp),\%ecx\n";
        print "\tcmpl \$0,\%ecx\n";
        print "\tje " . &$ref(1);
        print "\tmovl 8(\%esp),\%edx\n";
        print "\tsarl \%cl,\%edx\n";
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tmovl 16(\%esp),\%eax\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
        print "\tjmp " . &$ref(2);
        print &$label(1);
        print "\tbt \$31,\%edx\n";
        print "\tsetc \%cl\n";
        print "\tsarl \$31,\%edx\n";
        print "\tshll \$29,\%ecx\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
        print &$label(2);
      }
      else
      {
        print "\tmovl 12(\%esp),\%ecx\n";
        print "\tcmpl \$0,\%ecx\n";
        print "\tje " . &$ref(1);
        print "\tmovl 8(\%esp),\%edx\n";
        print "\tsarl \%cl,\%edx\n";
        print "\tjmp " . &$ref(2);
        print &$label(1);
        print "\tmovl 4(\%esp),\%eax\n";
        print "\tret\n";
        print &$label(2);
      }
    }
    elsif ($shifttype eq 'ror')
    {
      print "\tmovl 12(\%esp),\%ecx\n";
      print "\tmovl 8(\%esp),\%edx\n";
      print "\tmovl 16(\%esp),\%eax\n";
      print "\tcmpl \$0,\%ecx\n";
      print "\tje " . &$ref(1);
      print "\trorl \%cl,\%edx\n";
      print "\tjmp " . &$ref(2);
      print &$label(1);
      print "\tbt \$29,(\%eax)\n";
      print "\trcrl \$1,\%edx\n";
      print &$label(2);
      # carry valid for either ROR or RRX operation
      if ($flags)
      {
        print "\tsetc \%cl\n";
        print "\tshll \$29,\%ecx\n";
        print "\tbtr \$29,(\%eax)\n";
        print "\torl \%ecx,(\%eax)\n";
      }
    }
  }
}
