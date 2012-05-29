my $a;
my $b = 2;
$a = 4;
{
    my $c;
    my $d;
}
my $e;
{
    my $f = $a;
    my $g = $b;
	print($f, "\n");
	print($g, "\n");
    {
        my $h = $e;
        my $i = $f;
		print($h, "\n");
		print($i, "\n");
    }
    my $j = $g;
	print($j, "\n");
}
my $k;
print($k, "\n");
