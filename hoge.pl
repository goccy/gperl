sub f {
	$a = 2;
	$b = $a + 3 + 4;
	my $c = $a;
	my $n = shift;
	print $n + 1, "\n";
	print $a, "\n";
	print $b, "\n";
}

f(3);
