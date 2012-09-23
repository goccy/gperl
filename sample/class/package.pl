package Person;

sub new {
    my ($class, $name, $age) = @_;
    my $self = {
        age => $age,
        name => $name
    };
    return bless($self, $class);
}

sub get_name {
    my $self = shift;
    return $self->{name};
}

sub get_age {
    my $self = shift;
    return $self->{age};
}

package main;

my $person = Person->new("goccy", 25);
print($person, "\n");
print($person->get_name(), "\n");
print($person->get_age(), "\n");
