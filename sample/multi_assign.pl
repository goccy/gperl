sub f {
    my ($a, $b, $c, $d, $e) = @_;
    print($a, "\n");
    print($b, "\n");
    print($c, "\n");
    print($d, "\n");
    print($e, "\n");
}

my ($a, $b, $c, $d, $e) = (1, 2, 3, 4, 5);
print($a, "\n");
print($b, "\n");
print($c, "\n");
print($d, "\n");
print($e, "\n");
f($a, $b, $c, $d, $e);
f($a, $b, $c);
f($a, $b, $c, $d, $e, 6);
