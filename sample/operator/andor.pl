my $a = 2;
my $b = undef;

if (defined($a) && defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

if (!defined($a) && defined($b)) {
    print "true\n";
} else {
    print "false\n";
}
if (defined($a) && !defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

if (!defined($a) && !defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

if (defined($a) || defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

if (!defined($a) || defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

if (defined($a) || !defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

if (!defined($a) || !defined($b)) {
    print "true\n";
} else {
    print "false\n";
}

