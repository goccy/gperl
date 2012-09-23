sub f {
	my @args = @_;
	print(@args, "\n");
	push(@args, 5);
	return @args;
}

print(f(1, 2, 3, 4), "\n");
