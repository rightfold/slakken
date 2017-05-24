use strict;
use warnings;

open my $in, '<', 'src/opcode.def' or die $!;

open my $opcode_inc, '>', 'build/opcode.inc' or die $!;

while (<$in>) {
  my ($opcode, $mnemonic, @operands) = split /\s+/;
  next unless defined $opcode;

  my $mnemonic_ident = $mnemonic =~ s/\./_/gr;
  print $opcode_inc "$mnemonic_ident = $opcode,\n";
}
