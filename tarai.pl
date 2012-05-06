sub tarai{
	my $x = shift;
	my $y = shift;
	my $z = shift;
    if ($x <= $y) {
        return $y;
    } else {
        return &tarai(&tarai($x - 1, $y, $z),
					  &tarai($y - 1, $z, $x),
					  &tarai($z - 1, $x, $y));
    }
}

tarai(13, 6, 0);
