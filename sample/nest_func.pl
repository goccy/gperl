sub f {
    return 1 + $_[0];
}

print(f(f(1)), "\n");
