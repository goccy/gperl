package Vec;

sub new {
    my ($class, $x, $y, $z) = @_;
    my $self = {
        "x" => $x,
        "y" => $y,
        "z" => $z,
    };
    return bless($self, $class);
}

sub add {
    my $self = shift;
    my $v1 = shift;
    return Vec->new($self->{x} + $v1->{x},
                    $self->{y} + $v1->{y},
                    $self->{z} + $v1->{z});
}

sub sub_ {
    my $self = shift;
    my $v1 = shift;
    return Vec->new($self->{x} - $v1->{x},
                    $self->{y} - $v1->{y},
                    $self->{z} - $v1->{z});
}

sub dot {
    my $self = shift;
    my $v1 = shift;
    return $self->{x} * $v1->{x} + $self->{y} * $v1->{y} + $self->{z} * $v1->{z};
}

sub cross {
    my ($self, $v1) = @_;
    return Vec->new($self->{y} * $v1->{z} - $self->{z} * $v1->{y},
                    $self->{z} * $v1->{x} - $self->{x} * $v1->{z},
                    $self->{x} * $v1->{y} - $self->{y} * $v1->{x});
}

sub length {
    my $self = shift;
    return sqrt($self->dot($self));
}

sub normalize {
    my $self = shift;
    my $len = $self->length();
    if ($len > 1.0e-17) {
        $self->{x} /= $len;
        $self->{y} /= $len;
        $self->{z} /= $len;
    }
}

package Ray;

sub new {
    my ($class, $org, $dir) = @_;
    my $self = {
        org => $org,
        dir => $dir,
    };
    return bless($self, $class);
}

package Intersection;

sub new {
    my ($class) = @_;
    my $self = {
        t => 1.0e+17,
        p => undef,
        n => Vec->new(0.0, 0.0, 0.0),
        hit => 0,
    };
    return bless($self, $class);
}

package Sphere;
sub new {
    my ($class, $center, $radius) = @_;
    my $self = {
        center => $center,
        radius => $radius,
    };
    return bless($self, $class);
}

sub intersect {
    my ($self, $isect, $ray) = @_;
    my $rs = $ray->{org}->sub_($self->{center});
    my $b = $rs->dot($ray->{dir});
    my $c = $rs->dot($rs) - ($self->{radius} * $self->{radius});
    my $d = $b * $b - $c;
    if ($d > 0.0) {
        my $t = -$b - sqrt($d);
        if (($t > 0.0) && ($t < $isect->{t})) {
            $isect->{t} = $t;
            $isect->{hit} = 1;
            my $p = Vec->new($ray->{org}->{x} + $ray->{dir}->{x} * $t,
                             $ray->{org}->{y} + $ray->{dir}->{y} * $t,
                             $ray->{org}->{z} + $ray->{dir}->{z} * $t);
            my $n = $p->sub_($self->{center});
            $n->normalize();
        }
    }
}

package Plane;

sub new {
    my ($class, $p, $n) = @_;
    my $self = {
        p => $p,
        n => $n,
    };
    return bless($self, $class);
}

sub intersect {
    my ($self, $isect, $ray) = @_;
    my $d = -1 * $self->{p}->dot($self->{n});
    my $v = $ray->{dir}->dot($self->{n});
    if (abs($v) < 1.0e-17) {
        return;
    }
    my $t = -1 * ($ray->{org}->dot($self->{n}) + $d) / $v;
    if (($t > 0.0) && ($t < $isect->{t})) {
        $isect->{t} = $t;
        $isect->{hit} = 1;
        $isect->{n} = $self->{n};
        my $ip = Vec->new($ray->{org}->{x} + $t * $ray->{dir}->{x},
                          $ray->{org}->{y} + $t * $ray->{dir}->{y},
                          $ray->{org}->{z} + $t * $ray->{dir}->{z});
        $isect->{p} = $ip;
    }
}

package main;

my $WIDTH = 64;
my $HEIGHT = 64;
my $NSUBSAMPLES = 2;
my $NAO_SAMPLES = 8;
my $M_PI = atan2(1,1) * 4;
my @spheres;
my $plane;

sub ortho_basis {
    my ($basis, $n) = @_;
    $basis->[1] = Vec->new(0.0, 0.0, 0.0);
    $basis->[2] = Vec->new($n->{x}, $n->{y}, $n->{z});
    if (($n->{x} < 0.6) && ($n->{x} > -0.6)) {
        $basis->[1]->{x} = 1.0;
    } elsif (($n->{y} < 0.6) && ($n->{y} > -0.6)) {
        $basis->[1]->{y} = 1.0;
    } elsif (($n->{z} < 0.6) && ($n->{z} > -0.6)) {
        $basis->[1]->{z} = 1.0;
    } else {
        $basis->[1]->{x} = 1.0;
    }
    my $v0 = $basis->[0];
    my $v1 = $basis->[1];
    my $v2 = $basis->[2];
    $v0 = $v1->cross($v2);
    $v0->normalize();
    $v1 = $v2->cross($v0);
    $v1->normalize();
    $basis->[0] = $v0;
    $basis->[1] = $v1;
}

sub clamp {
    my $f = shift;
    my $i = int($f * 255.5);
    if ($i < 0) {
        $i = 0;
    }
    if ($i > 255) {
        $i = 255;
    }
    return $i;
}

sub ambient_occlusion {
    my $isect = shift;
    my $ntheta = $NAO_SAMPLES;
    my $nphi = $NAO_SAMPLES;
    my $eps = 0.0001;

    my $p = Vec->new($isect->{p}->{x} + $eps * $isect->{n}->{x},
                     $isect->{p}->{y} + $eps * $isect->{n}->{y},
                     $isect->{p}->{z} + $eps * $isect->{n}->{z});
    my @basis = ();
    ortho_basis(\@basis, $isect->{n});
    my $occlusion = 0.0;

    for (my $j = 0; $j < $ntheta; $j++) {
        for (my $i = 0; $i < $nphi; $i++) {
            my $r = rand();
            my $theta = 1 - sqrt(rand());
            my $phi = 2.0 * $M_PI * rand();
            my $x = cos($phi) * sqrt(1 - $r);
            my $y = sin($phi) * sqrt(1 - $r);;
            my $z = sqrt($r);

            my $rx = $x * $basis[0]->{x} + $y * $basis[1]->{x} + $z * $basis[2]->{x};
            my $ry = $x * $basis[0]->{y} + $y * $basis[1]->{y} + $z * $basis[2]->{y};
            my $rz = $x * $basis[0]->{z} + $y * $basis[1]->{z} + $z * $basis[2]->{z};

            my $ray = Ray->new($p, Vec->new($rx, $ry, $rz));

            my $occIsect = Intersection->new();
            $spheres[0]->intersect($occIsect, $ray);
            $spheres[1]->intersect($occIsect, $ray);
            $spheres[2]->intersect($occIsect, $ray);
            $plane->intersect($occIsect, $ray);
            if ($occIsect->{hit}) {
                $occlusion += 1.0;
            }
        }
    }

    $occlusion = ($ntheta * $nphi - $occlusion) / ($ntheta * $nphi);
    return Vec->new($occlusion, $occlusion, $occlusion);
}

sub render {
    my ($img, $w, $h, $nsubsamples) = @_;
    my @fimg;
    for (my $y = 0; $y < $h; $y++) {
        for (my $x = 0; $x < $w; $x++) {
            for (my $v = 0; $v < $nsubsamples; $v++) {
                for (my $u = 0; $u < $nsubsamples; $u++) {
                    my $px = ($x + ($u / $nsubsamples) - ($w / 2.0)) / ($w / 2.0);
                    my $py = -1 * ($y + ($v / $nsubsamples) - ($h / 2.0)) / ($h / 2.0);
                    my $eye = Vec->new($px, $py, -1.0);
                    $eye->normalize();
                    my $ray = Ray->new(Vec->new(0.0, 0.0, 0.0), $eye);

                    my $isect = Intersection->new();

                    $spheres[0]->intersect($isect, $ray);
                    $spheres[1]->intersect($isect, $ray);
                    $spheres[2]->intersect($isect, $ray);
                    $plane->intersect($isect, $ray);

                    if ($isect->{hit}) {
                        my $c = ambient_occlusion($isect);
                        my $p0 = 3 * ($y * $w + $x);
                        $fimg[$p0 + 0] += $c->{x};
                        $fimg[$p0 + 1] += $c->{y};
                        $fimg[$p0 + 2] += $c->{z};
                    }
                }
            }
            my $p1 = 3 * ($y * $w + $x);
            $fimg[$p1 + 0] /= ($nsubsamples * $nsubsamples);
            $fimg[$p1 + 1] /= ($nsubsamples * $nsubsamples);
            $fimg[$p1 + 2] /= ($nsubsamples * $nsubsamples);
            $img->[$p1 + 0] = clamp($fimg[$p1 + 0]);
            $img->[$p1 + 1] = clamp($fimg[$p1 + 1]);
            $img->[$p1 + 2] = clamp($fimg[$p1 + 2]);
        }
    }
}

my @img;
$spheres[0] = Sphere->new(Vec->new(-2.0, 0.0, -3.5), 0.5);
$spheres[1] = Sphere->new(Vec->new(-0.5, 0.0, -3.0), 0.5);
$spheres[2] = Sphere->new(Vec->new(1.0, 0.0, -2.2), 0.5);
$plane = Plane->new(Vec->new(0.0, -0.5, 0.0), Vec->new(0.0, 1.0, 0.0));
render(\@img, $WIDTH, $HEIGHT, $NSUBSAMPLES);
open(FP, ">", "ao.ppm");
binmode(FP);
print FP "P6\n$WIDTH $HEIGHT\n255\n";
foreach (@img) {
    print FP chr($_);
}
close(FP);
