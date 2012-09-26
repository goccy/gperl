sub f {
    sub g {}
}

for (my $i = 0; $i < 300000; $i++) {
    f();
}
