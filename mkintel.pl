#!/usr/bin/perl

# Take a couple of x86 definition files and output some C code?

$input = "intel.dat";

open(input);
@inteldat = <input>;
close(input);

print "#include \"x86asm.h\"\n";
print "#include \"rtasm.h\"\n";

foreach (@inteldat)
{
  chop;
  s/#.*//g;      # strip comments
  if (!m/^\s*$/)  # no empty lines
  {
    ($lhs,$rhs) = split(/\|/);
    $lhs =~ s/\s*$//;
    @lhs = split(/[,\s]/, $lhs);  # I *think* this should be OK
    @rhs = split(/[,\s]/, $rhs);
    if ($rhs =~ m/xmm/) {
      $type = 'sse';
    }
    elsif ($rhs =~ m/[^i]mm/) {
      $type = 'mmx';
    }
    else {
      $type = '';
    }
    # lets just not support all those extra instruction sets, OK?
    if ($type ne '') { next; }
    print "/* $rhs */\n";
    @args = ("nativeblockinfo* nat");
    $narg = @args;
    $extra = '';
    $immno = '';
    $fnname = "rtasm";
    foreach (@rhs)
    {
      if (m/^E?A[LX]/) {
        # add no args for AL/AX/EAX
        $fnname .= "_".lc $_;
      }
      elsif (m/^imm8$/) {
        @args[$narg++] = "uint3 imm".$immno;
        $fnname .= "_$_";
        $immno = bump($immno);
      }
      elsif (m/^imm16$/) {
        @args[$narg++] = "uint4 imm".$immno;
        $fnname .= "_$_";
        $immno = bump($immno);
      }
      elsif (m/^imm32$/) {
        @args[$narg++] = "uint5 imm".$immno;
        $fnname .= "_$_";
        $immno = bump($immno);
      }
      elsif (m/^r(32)?\/m(8|16|32)$/) {
        @args[$narg++] = "rtasm_mtype mem";
        s/\///;
        $fnname .= "_$_";
      }
      elsif (m/^r(8|16|32)$/) {
        @args[$narg++] = "uint3 reg";
        $fnname .= "_$_";
      }
      elsif (m/^rel8$/) {
        @args[$narg++] = "uint3 offset";
        $fnname .= "_$_";
      }
      elsif (m/^rel16$/) {
        @args[$narg++] = "uint4 offset";
        $fnname .= "_$_";
      }
      elsif (m/^(rel32|rel16\/32)$/) {
        @args[$narg++] = "uint5 offset";
        $fnname .= "_rel32";
      }
      elsif (m/^m(8|16|32|64)$/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_$_";
      }
      elsif (m/^m(16&16|32&32)$/) {
        @args[$narg++] = "rtasm_mtype mem";
        s/&16|&32/x2/;
        $fnname .= "_$_";
      }
      elsif (m/^m16&32$/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_m16n32";
      }
      elsif (m/^DR$/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_dbg";
      }
      elsif (m/^CR$/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_cr" . substr($_,2,1);
      }
      elsif (m/^(mm|Sreg)$/) {
        @args[$narg++] = "uint5 reg";
        $fnname .= "_$_";
      }
      elsif (m/^mm.*\/m(16|32|64)/) {
        @args[$narg++] = "rtasm_mtype mem";
        s/\///;
        $fnname .= "_$_";
      }
      elsif (m/^CL$/) {
        # cl doesn't need to be passed
        $fnname .= "_cl";
      }
      elsif (m/^1$/) {
        # 1 doesn't need to be passed
        $fnname .= "_1";
      }
      elsif (m/^ptr16:16$/) {
        @args[$narg++] = "uint4 segment";
        @args[$narg++] = "uint4 offset";
        $extra = "\toffset = segment | (offset<<16);\n";
        s/:/_/;
        $fnname .= "_$_";
      }
      elsif (m/^ptr16:32$/) {
        @args[$narg++] = "uint4 segment";
        @args[$narg++] = "uint5 offset";
        s/:/_/;
        $fnname .= "_$_";
      }
      elsif (m/^m16\:(16|32)/) {
        @args[$narg++] = "rtasm_mtype mem";
        s/:/_/;
        $fnname .= "_$_";
      }
      elsif (m/^m(2|16|32|64)(byte|int|real)/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_$_";
      }
      elsif (m/^m80(dec|bcd|real)/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_$_";
      }
      elsif (m/^m(14\/28|94\/108|512)byte/) {
        @args[$narg++] = "rtasm_mtype mem";
        s/\///;
        $fnname .= "_$_";
      }
      elsif (m/^m$/) {
        @args[$narg++] = "rtasm_mtype mem";
        $fnname .= "_$_";
      }
      elsif (m/^ST\(0\)/) {
        $fnname .= "_st0";
      }
      elsif (m/^ST\(i\)/) {
        @args[$narg++] = "uint3 stackn";
        $fnname .= "_sti";
      }
      elsif (m/^xmm(2)?\/.*/) {
        @args[$narg++] = "rtasm_mtype mem";
        s/\///;
        $fnname .= "_$_";
      }
      elsif (m/^xmm(1)?$*/) {
        @args[$narg++] = "uint3 reg";
        $fnname .= "_$_";
      }
      else {
        $fnname .= lc "_$_";
      }
    }

    $" = ", ";
    print 'void '.$fnname."(@args)\n{\n";

    $opcode = $modrm = $mem = $immed = $immno = "";

    foreach (@lhs)
    {
      if (m/^[0-9A-F][0-9A-F]$/) {
        $opcode = $opcode . "\trtasm_putbyte(nat, 0x$_);\n";
      }
      elsif (m/^[0-9A-F][0-9A-F]\+r[bdw]/) {
        $opcode = $opcode . "\trtasm_putbyte(nat, 0x".substr($_,0,2)."+reg);\n";
      }
      elsif (m/^[0-9A-F][0-9A-F]\+i/) {
        $opcode = $opcode .
          "\trtasm_putbyte(nat, 0x".substr($_,0,2)."+stackn);\n";
      }
      elsif (m/\/r/) {
        # get mode
        $modrm = "\trtasm_putbyte(nat, mem.rm | (reg<<3) | (mem.mod<<6));\n";
        $mem = "\trtasm_mem(nat, &mem);\n";
      }
      elsif (m/\/[0-7]/) {
        s/\///;
        $modrm = "\trtasm_putbyte(nat, mem.rm | ".($_<<3)." | " .
                 "(mem.mod<<6));\n";
        $mem = "\trtasm_mem(nat, &mem);\n";
      }
      elsif (m/\/sf/) {
        $modrm = "\trtasm_putbyte(nat, 0xF8);\n";
      }
      elsif (m/ib/) {
        $immed .= "\trtasm_putbyte(nat, imm$immno);\n";
        $immno = bump($immno);
      }
      elsif (m/iw/) {
        $immed .= "\trtasm_putbyte(nat, imm$immno \& 0xff);\n" .
                  "\trtasm_putbyte(nat, imm$immno>>8);\n";
        $immno = bump($immno);
      }
      elsif (m/id/) {
        $immed .= "\trtasm_putbyte(nat, imm$immno \& 0xff);\n" .
                  "\trtasm_putbyte(nat, (imm$immno>>8) \& 0xff);\n" .
                  "\trtasm_putbyte(nat, (imm$immno>>16) \& 0xff);\n" .
                  "\trtasm_putbyte(nat, imm$immno>>24);\n";
        $immno = bump($immno);
      }
      elsif (m/cb/) {
        $immed .= "\trtasm_putbyte(nat, offset);\n";
      }
      elsif (m/cw/) {
        $immed .= "\trtasm_putbyte(nat, offset \& 0xff);\n" .
                  "\trtasm_putbyte(nat, offset>>8);\n";
      }
      elsif (m/cd/) {
        $immed .= "\trtasm_putbyte(nat, offset \& 0xff);\n" .
                  "\trtasm_putbyte(nat, (offset>>8) \& 0xff);\n" .
                  "\trtasm_putbyte(nat, (offset>>16) \& 0xff);\n" .
                  "\trtasm_putbyte(nat, offset>>24);\n";
      }
      elsif (m/cp/) {
        $immed .= "\trtasm_putbyte(nat, segment \& 0xff);\n" .
                  "\trtasm_putbyte(nat, segment>>8);\n" .
                  "\trtasm_putbyte(nat, offset \& 0xff);\n" .
                  "\trtasm_putbyte(nat, (offset>>8) \& 0xff);\n" .
                  "\trtasm_putbyte(nat, (offset>>16) \& 0xff);\n" .
                  "\trtasm_putbyte(nat, offset>>24);\n";
      }
    }
    # output function body
    print "$extra$opcode$modrm$mem$immed}\n";
  }
}

sub bump {
  if ($_[0] eq '') {
    return 2;
  }
  else {
    return $_[0]+1;
  }
}

