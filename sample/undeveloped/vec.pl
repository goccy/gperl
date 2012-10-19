package Vec;

sub new {
    my ($class, $x, $y, $z) = @_;
    my $self = {
        "x" => $x,
        "y" => $y,
        "z" => $z
    };
    return bless($self, $class);
}

sub add {
    my $self = shift;
    my $v1 = shift;
    return Vec->new(($self->{x}) + ($v1->{x}),
                    ($self->{y}) + ($v1->{y}),
                    ($self->{z}) + ($v1->{z}));
}

sub sub_ {
    my $self = shift;
    my $v1 = shift;
    return Vec->new(($self->{x}) - ($v1->{x}),
                    ($self->{y}) - ($v1->{y}),
                    ($self->{z}) - ($v1->{z}));
}

sub dot {
    my $self = shift;
    my $v1 = shift;
    return (($self->{x}) * ($v1->{x})) + (($self->{y}) * ($v1->{y})) + (($self->{z}) * ($v1->{z}));
}

sub cross {
    my ($self, $v1) = @_;
    return Vec->new((($self->{y}) * ($v1->{z})) - (($self->{z}) * ($v1->{y})),
                    (($self->{z}) * ($v1->{x})) - (($self->{x}) * ($v1->{z})),
                    (($self->{x}) * ($v1->{y})) - (($self->{y}) * ($v1->{x})));
}

sub length {
    my $self = shift;
    return sqrt($self->dot($self));
}

sub normalize {
    my $self = shift;
    my $len = $self->length();
    if ($len > 1.0e-17) {
        ($self->{x}) /= $len;
        ($self->{y}) /= $len;
        ($self->{z}) /= $len;
    }
}
