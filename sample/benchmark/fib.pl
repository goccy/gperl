#@static_typing
sub fib {
	if ($_[0] < 3) {
		return 1;
	} else {
		return fib($_[0] - 1) + fib($_[0] - 2);
	}
}

print(fib(36), "\n");
