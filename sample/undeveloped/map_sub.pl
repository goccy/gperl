sub f {
    my @recv = @_;
    my @ret;
    foreach (@recv) {
        push(@ret, $_ * 2);
    }
    return @ret;
}

my @a = (1, 2, 3, 4);
my @b = f(1, 2, 3, 4);
print(@b, "\n");
