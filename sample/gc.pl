my @a = (1, 2, 3, 4);
my $s = "h";
for (my $i = 0; $i < 100000; $i++) {
	$s = "h";
	push(@a, $s);
}
