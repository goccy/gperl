sub f {
    print($_[0] + 1, "\n");
}

my $code = \&f;
&$code(3);
&$code(4);
