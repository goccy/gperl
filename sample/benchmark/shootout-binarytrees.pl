#@static_typing
sub bottomup_tree {
    my $depth = $_[1];
    if (!$depth) {
        return [undef, undef, $_[0]];
    } else {
        my $value2 = $_[0] * 2;
        $depth--;
        return [bottomup_tree($value2 - 1, $depth), bottomup_tree($value2, $depth), $_[0]];
    }
}

#@jit_safe
sub check_tree {
    unless (defined($_[0]->[0])) {
        return ($_[0]->[2]);
    } else {
        return check_tree($_[0]->[0]) - check_tree($_[0]->[1]) + ($_[0]->[2]);
    }
}

my $max_depth = 14;
my $min_depth = 4;

if ($min_depth + 2 > $max_depth) {
    $max_depth = $min_depth + 2;
}

my $stretch_depth = $max_depth + 1;
my $stretch_tree = bottomup_tree(0, $stretch_depth);
my $check = check_tree($stretch_tree);
print("stretch tree of depth ", $stretch_depth, "\t check: ", $check, "\n");
my $longlived_tree = bottomup_tree(0, $max_depth);

for (my $depth = $min_depth; $depth <= $max_depth; $depth += 2) {
    my $iterations = 2 << ($max_depth - $depth + $min_depth - 1);
    $check = 0;
    for (my $i = 1; $i <= $iterations; $i++) {
        $check += check_tree(bottomup_tree(0, $depth));
        $check += check_tree(bottomup_tree(0, $depth));
    }
    print(2 * $iterations, "\t trees of depth ", $depth, "\t check: ", $check, "\n");
}

print("long lived tree of depth ", $max_depth, "\t check: ", check_tree($longlived_tree), "\n");
