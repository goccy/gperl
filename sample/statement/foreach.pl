my @a = (1, 2, 3, 4);
my $i = 1;
foreach my $i (@a) {
    print($i, "\n");
}
print($i, "\n");

foreach (@a) {
    print($_, "\n");
}

for (@a) {
    print($_, "\n");
}

my @b = (5, 6, 7, 8);
foreach (@a) {
    print($_);
    foreach (@b) {
        print($_);
    }
    print("\n");
}
