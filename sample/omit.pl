sub f {
    print "f = ", @_, "\n";
    return 10;
}

sub g {
    print "g = ", @_, "\n";
    return 5;
}

print "fuga = ", f 1, 2, g;
print "fuga = ", f 1, 2, g, "\n";
print "hoge = ", f 1, 2, g 2, 3, "\n", 4;
