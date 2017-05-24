use strict;
use warnings;

open my $in, '<', 'src/opcode.def' or die $!;

open my $opcode_inc, '>', 'build/opcode.inc' or die $!;
open my $decode_inc, '>', 'build/decode.inc' or die $!;

sub write_opcode {
  my ($opcode, $mnemonic) = @_;
  my $mnemonic_ident = $mnemonic =~ s/\./_/gr;
  print $opcode_inc "$mnemonic_ident = $opcode,\n";
}

sub write_decode {
  my ($opcode, $mnemonic, @operands) = @_;
  my $mnemonic_ident = $mnemonic =~ s/\./_/gr;
  print $decode_inc "if (*begin == $opcode) {\n";
  print $decode_inc "  ++begin;\n";
  print $decode_inc "  instruction inst;\n";
  print $decode_inc "  inst.what = opcode::$mnemonic_ident;\n";
  for my $i (0 .. $#operands) {
    my $operand = $operands[$i];
    if ($operand eq 'imm') {
      print $decode_inc "  auto value = decode<std::uint32_t>(begin, end);\n";
      print $decode_inc "  inst.op$i.imm = value;\n";
    }
    if ($operand eq 'val') {
      print $decode_inc "  auto index = decode<std::uint32_t>(begin, end);\n";
      print $decode_inc "  inst.op$i.val = consts.at(index);\n";
    }
  }
  print $decode_inc "  insts.push_back(inst);\n";
  print $decode_inc "  continue;\n";
  print $decode_inc "}\n";
}

while (<$in>) {
  my ($opcode, $mnemonic, @operands) = split /\s+/;
  next unless defined $opcode;

  write_opcode $opcode, $mnemonic;
  write_decode $opcode, $mnemonic, @operands;
}
