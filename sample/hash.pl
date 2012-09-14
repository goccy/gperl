my %hash1 = ("a" => 3, "b" => 1.2345678, "c" => "hoge");
print($hash1{"a"}, "\n");
print($hash1{"b"}, "\n");
print($hash1{"c"}, "\n");
print(%hash1, "\n");

my %hash2 = (a => 3, b => 1.2345678, c => "hoge");
print($hash2{a}, "\n");
print($hash2{b}, "\n");
print($hash2{c}, "\n");
print($hash2{d}, "\n");

my @a = ("a" => 3, "b" => 1.2345678, "c" => "hoge");
print(@a, "\n");

my @c = keys(%hash1);
print("keys = ", @c, "\n");
my @d = values(%hash1);
print("values = ", @d, "\n");

=todo
foreach (keys(%hash2)) {
    print($_, "\n");
}
=cut
