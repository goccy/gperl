my @a = (1, 2, 3, 4);
my $ref_a = [1, 2, 3, 4];
if (ref(@a)) {
    print("true\n");
} else {
    print("false\n");
}

if (ref($ref_a)) {
    print("true\n");
} else {
    print("false\n");
}
