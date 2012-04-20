sub fib {
	my $n = shift;
	print $n, "\n";
	if ($n < 3) {
		print "true\n";
		return 1;
	} else {
		print "false\n";
		return 2;
	}
}

my $a = 4;
fib($a - 1) + fib($a - 2);
