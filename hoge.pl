sub fib {
	my $n = shift;
	if ($n < 3) {
		return 1;
	} else {
		return 2;
	}
}

fib(2);
fib(3);
