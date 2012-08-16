sub g {
    return "hello";
}

sub f {
	return g();
}

print(f(), "\n");
