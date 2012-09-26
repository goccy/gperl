#@static_typing
sub bottomup_tree {
    my $value = $_[0];
    my $depth = $_[1];
    if (!$depth) {
        return $value;
    } else {
        my $value2 = $value * 2;
        $depth--;
        return [bottomup_tree($value - 1, $depth), bottomup_tree($value2, $depth), $value];
    }
}

#@static_typing
sub check_tree {
    my $ref_arg = shift;
    my @deref_arg = @{$ref_arg};
    my $left = $deref_arg[0];
    my $right = $deref_arg[1];
    my $value = $deref_arg[2];
    my $value2;
    if (ref($left)) {
        $value2 = check_tree($left) - check_tree($right);
    } else {
        $value2 = $left - $right;
    }
    return $value + $value2;
}

my $max_depth = 15;
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
    for (my $i = 1; $i < $iterations; $i++) {
        $check += check_tree(bottomup_tree(0, $depth));
        $check += check_tree(bottomup_tree(0, $depth));
    }
    print(2 * $iterations, "\t trees of depth ", $depth, "\t check: ", $check, "\n");
}

print("long lived tree of depth ", $max_depth, "\t check: ", check_tree($longlived_tree), "\n");
