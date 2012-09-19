my $hash = {"a" => 3, "b" => 1.2345678, "c" => "hoge"};
print($hash->{"a"}, "\n");
print($hash->{"b"}, "\n");
print($hash->{"c"}, "\n");
print($hash, "\n");

my %dereferenced_hash = %{$hash};
print($dereferenced_hash{"a"}, "\n");
print($dereferenced_hash{"b"}, "\n");
print($dereferenced_hash{"c"}, "\n");
print(%dereferenced_hash, "\n");
