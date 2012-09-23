sub tarai{
    if ($_[0] <= $_[1]) {
        return $_[1];
    } else {
        return &tarai(&tarai($_[0] - 1, $_[1], $_[2]),
					  &tarai($_[1] - 1, $_[2], $_[0]),
					  &tarai($_[2] - 1, $_[0], $_[1]));
    }
}

print(tarai(13, 6, 0), "\n");
