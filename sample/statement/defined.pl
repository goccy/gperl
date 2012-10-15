my $a = undef;
if (!$a) {
    print("defined\n");
} else {
    print("undefined\n");
}

my $b = [1];
if (!defined($b)) {
    print("defined\n");
} else {
    print("undefined\n");
}
