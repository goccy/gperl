use JSON qw/encode_json decode_json/;
use Data::Dumper;
use HTML::Template;

sub read_code_data {
    my $file_name = shift;
    open(IN, "<" . $file_name);
    my $json_data;
    foreach (<IN>) {
        $json_data .= $_;
    }
    my $data = decode_json($json_data);
    return $data;
}

sub add_type_inst {
    my $data = $_[0];
    my $name = $_[1];
    my $names = $data->{names};
    push(@$names, {name => "STATIC_${name}", static_flag => 1});
    push(@$names, {name => "d${name}", prefix => "d"});
    push(@$names, {name => "i${name}", prefix => "i"});
    if ($name !~ /SUB/ && $name !~ /DIV/) {
        push(@$names, {name => "s${name}", prefix => "s"});
    }
    push(@$names, {name => "o${name}", prefix => "o"});
}

sub add_fast_type_inst {
    my ($data, $name, $fast_prefix) = @_;
    my $fast_names = $data->{fast_names};
    my $const_flag = ($name =~ /C$/) ? 1 : 0;
    foreach my $prefix (@$fast_prefix) {
        my $prefix1 = substr($prefix, 0, 1);
        my $prefix2 = substr($prefix, 1, 1);
        if ($data->{type_flag}) {
            push(@$fast_names, {name => "${prefix}_${name}", prefix1 => $prefix1, prefix2 => $prefix2, check_flag => 1, const_flag => $const_flag});
            unless ($const_flag) {
                push(@$fast_names, {name => "${prefix}_STATIC_${name}", prefix1 => $prefix1, prefix2 => $prefix2, static_flag => 1, const_flag => $const_flag});
            }
            push(@$fast_names, {name => "${prefix}_d${name}", prefix1 => $prefix1, prefix2 => $prefix2, prefix => "d", const_flag => $const_flag});
            push(@$fast_names, {name => "${prefix}_i${name}", prefix1 => $prefix1, prefix2 => $prefix2, prefix => "i", const_flag => $const_flag});
            if ($name !~ /SUB/ && $name !~ /DIV/) {
                push(@$fast_names, {name => "${prefix}_s${name}", prefix1 => $prefix1, prefix2 => $prefix2,  prefix => "s", const_flag => $const_flag});
            }
            unless ($const_flag) {
                push(@$fast_names, {name => "${prefix}_o${name}", prefix1 => $prefix1, prefix2 => $prefix2, prefix => "o", const_flag => $const_flag});
            }
        } else {
            push(@$fast_names, {name => "${prefix}_${name}", prefix1 => $prefix1, prefix2 => $prefix2, prefix => $prefix, check_flag => 1, const_flag => $const_flag});
        }
    }
}

sub add_const_type_inst {
    my $data = $_[0];
    my $names = $data->{names};
    my $name = $data->{name};
    my $fast_prefix = ["A", "B", "C", "D"];
    if ($data->{type_flag}) {
        push(@$names, {name => "d${name}C", prefix => "d", const_flag => 1});
        push(@$names, {name => "i${name}C", prefix => "i", const_flag => 1});
        add_fast_type_inst($data, $name . "C", $fast_prefix) if ($data->{fast_type});
    } else {
        push(@$names, "${name}C");
        add_fast_type_inst($data, $name . "C", $fast_prefix) if ($data->{fast_type});
    }
}

sub get_inst_data {
    my $code_info = $_[0];
    my $inst_data = [];
    foreach $info (@$code_info) {
        my $inst_name = $info->{code};
        my $type_flag = $info->{type};
        my $const_flag = $info->{const};
        my $fast_type = $info->{fast_type};
        my @args = $info->{args};
        my $type = $info->{op_type};

        my @inst_names = ();
        my @fast_inst_names = ();
        my @fast_prefix = ($fast_type == 1) ? qw{AB AC AD BC BD CD} : qw{A B C D};
        my $data = {
            name => $inst_name,
            names => \@inst_names,
            fast_names => \@fast_inst_names,
            fast_prefix => \@fast_prefix,
            fast_type => $fast_type,
            type_flag => $type_flag,
            args => \@args,
            op_type => $type
        };
        push(@inst_names, {name => ${inst_name}, check_flag => 1});
        add_type_inst($data, $inst_name) if ($type_flag);
        add_fast_type_inst($data, $inst_name, $data->{fast_prefix}) if ($fast_type);
        add_const_type_inst($data) if ($const_flag);
        push(@$inst_data, $data);
    }
    return $inst_data;
}

sub gen_enum_code {
    my $inst_data = $_[0];
    my @opcodes;
    my @types;
    foreach my $data (@$inst_data) {
        push(@opcodes, {code => $_->{name}}) foreach (@{$data->{names}});
    }
    foreach my $data (@$inst_data) {
        push(@opcodes, {code => $_->{name}}) foreach (@{$data->{fast_names}});
    }
    my $type_map = {};
    foreach my $data (@$inst_data) {
        my $type = $data->{op_type};
        $type_map->{$type}++;
    }
    foreach my $type (keys(%$type_map)) {
        push(@types, {type => $type});
    }
    my $tmpl = HTML::Template->new(filename => "gen/tmpl/gen_vmcode.tmpl");
    $tmpl->param(opcodes => \@opcodes);
    $tmpl->param(types => \@types);
    return $tmpl->output;
}

sub gen_decl_code {
    my $inst_data = $_[0];
    my @decl;
    foreach my $data (@$inst_data) {
        foreach my $inst (@{$data->{names}}) {
            my $name = $inst->{name};
            push(@decl, {code => $name, name => $name, type => $data->{op_type}});
        }
    }
    foreach my $data (@$inst_data) {
        foreach my $inst (@{$data->{fast_names}}) {
            my $name = $inst->{name};
            push(@decl, {code => $name, name => $name, type => $data->{op_type}});
        }
    }
    my $tmpl = HTML::Template->new(filename => "gen/tmpl/gen_decl_code.tmpl");
    $tmpl->param(decl => \@decl);
    return $tmpl->output;
}

sub gen_label_code {
    my $inst_data = $_[0];
    my @labels;
    foreach my $data (@$inst_data) {
        my @names = @{$data->{names}};
        my @fast_names = @{$data->{fast_names}};
        for (my $i = 0; $i <= $#names; $i += 3) {
            my $label = {};
            $label->{inst1} = $names[$i]->{name}   if (defined $names[$i]);
            $label->{inst2} = $names[$i+1]->{name} if (defined $names[$i+1]);
            $label->{inst3} = $names[$i+2]->{name} if (defined $names[$i+2]);
            push(@labels, $label);
        }
        for (my $i = 0; $i <= $#fast_names; $i += 3) {
            my $label = {};
            $label->{inst1} = $fast_names[$i]->{name}   if (defined $fast_names[$i]);
            $label->{inst2} = $fast_names[$i+1]->{name} if (defined $fast_names[$i+1]);
            $label->{inst3} = $fast_names[$i+2]->{name} if (defined $fast_names[$i+2]);
            push(@labels, $label);
        }
    }
    my $tmpl = HTML::Template->new(filename => "gen/tmpl/gen_label.tmpl");
    $tmpl->param(labels => \@labels);
    return $tmpl->output;
}

sub get_args {
    my ($name, $args, $prefix1, $prefix2) = @_;
    my $fast_prefix_map = { A => 0, B => 1, C => 2, D => 3};
    my $SPECIFIC_INST_LIST = qw{RET SHIFT};
    my @deref_args = @{@$args[0]};
    my @values;
    foreach my $arg (@deref_args) {
        my @keys = keys(%$arg);
        my $key = $keys[0];
        my $value = $arg->{$key};
        $value = $fast_prefix_map->{$prefix1} if ($value =~ /dst/ && exists $fast_prefix_map->{$prefix1});
        $value = $fast_prefix_map->{$prefix2} if ($value =~ /src/ && exists $fast_prefix_map->{$prefix2});
        $value = $fast_prefix_map->{$prefix1} if (($name =~ /RET/ || $name =~ /SHIFT/) && $value =~ /src/ && exists $fast_prefix_map->{$prefix1});
        $value = $name if ($value eq "callname");
        push(@values, $value);
    }
    return join(", ", @values);
}

sub get_write_data {
    my $inst = $_[0];
    my $decl_args = $_[1];
    my @SINGLE_CHECK_LIST = qw{IS ISNOT WRITE INC gINC DEC gDEC};
    my @DOUBLE_CHECK_LIST = qw{ADD SUB MUL DIV LSHIFT RSHIFT JL JG JLE JGE JE JNE};
    my @CONST_CHECK_LIST = qw{IS ISNOT JL JG JLE JGE JE JNE ADD SUB MUL DIV LSHIFT RSHIFT};
    my @CMP_JMP_LIST = qw{JL JG JLE JGE JE JNE};
    my @NO_USE_REGISTER_LIST = qw{INC DEC};
    my @NO_PC_INCREMENT_LIST = ("IS", "ISNOT", @CMP_JMP_LIST);
    my $name = $inst->{name};
    my $prefix1 = $inst->{prefix1};
    my $prefix2 = $inst->{prefix2};
    my $op = $name;
    $inst->{use_register_flag} = 1;
    $inst->{pc_increment_flag} = 1;
    my $args = get_args($name, $decl_args, $prefix1, $prefix2);
    if ($inst->{const_flag} && grep {$name =~ $_} @CONST_CHECK_LIST) {
        $args =~ s/pc->src/pc->v/;
    }
    if ($inst->{static_flag}) {
        if (grep {$name =~ $_} @DOUBLE_CHECK_LIST) {
            $inst->{double_static_type_check_flag} = 1;
        } else {
            $inst->{single_static_type_check_flag} = 1;
            if (grep {$name =~ $_} @NO_USE_REGISTER_LIST) {
                ($name =~ /_g/) ? $inst->{use_global_flag} = 1 : $inst->{use_stack_flag} = 1;
                $inst->{use_register_flag} = 0;
            }
        }
    } elsif (grep {$name =~ $_} @DOUBLE_CHECK_LIST) {
        $inst->{double_type_check_flag} = 1;
    } elsif (grep {$name =~ $_} @SINGLE_CHECK_LIST) {
        $inst->{single_type_check_flag} = 1;
        if (grep {$name =~ $_} @NO_USE_REGISTER_LIST) {
            ($name =~ /g/) ? $inst->{use_global_flag} = 1 : $inst->{use_stack_flag} = 1;
            $inst->{use_register_flag} = 0;
        }
    } elsif (grep {$name =~ $_} @CMP_JMP_LIST) {
        $name = $inst->{prefix} . "CMP_JMP";
        if ($inst->{const_flag}) {
            $name .= "C";
        } elsif (!$inst->{prefix}) {
            $inst->{double_type_check_flag} = 1;
        }
    }
    if (grep {$op =~ $_} @NO_PC_INCREMENT_LIST) {
        $inst->{pc_increment_flag} = 0;
    }
    if (!defined($inst->{use_global_flag}) && !defined($inst->{use_stack_flag}) &&
        !defined($inst->{single_type_check_flag}) && !defined($inst->{single_static_type_check_flag}) &&
        !defined($inst->{double_type_check_flag}) && !defined($inst->{double_static_type_check_flag})) {
        $inst->{normal_flag} = 1;
    }
    $name =~ s/${prefix1}${prefix2}_// if ($prefix1);
    return {
        op                            => $op,
        func                          => $name,
        args                          => $args,
        use_register_flag             => $inst->{use_register_flag},
        use_global_flag               => $inst->{use_global_flag},
        use_stack_flag                => $inst->{use_stack_flag},
        single_type_check_flag        => $inst->{single_type_check_flag},
        single_static_type_check_flag => $inst->{single_static_type_check_flag},
        double_type_check_flag        => $inst->{double_type_check_flag},
        double_static_type_check_flag => $inst->{double_static_type_check_flag},
        normal_flag                   => $inst->{normal_flag},
        pc_increment_flag             => $inst->{pc_increment_flag}
    };
}

sub gen_vm_run_code {
    my $inst_data = $_[0];
    my @vm_codes;
    my @SPECIFIC_INST_LIST = qw{THCODE KEYS VALUES EACH_STEP EACH_LET};
    foreach my $data (@$inst_data) {
        next if (grep {$data->{name} eq $_} @SPECIFIC_INST_LIST);
        foreach my $inst (@{$data->{names}}) {
            push(@vm_codes, get_write_data($inst, $data->{args}));
        }
    }
    my $tmpl = HTML::Template->new(filename => "gen/tmpl/gen_vm.tmpl");
    $tmpl->param(vm_codes => \@vm_codes);
    return $tmpl->output;
}

sub gen_fast_vm_code {
    my $inst_data = $_[0];
    my @fast_vm_codes;
    foreach my $data (@$inst_data) {
        foreach my $inst (@{$data->{fast_names}}) {
            push(@fast_vm_codes, get_write_data($inst, $data->{args}));
        }
    }
    my $tmpl = HTML::Template->new(filename => "gen/tmpl/gen_fast_vmcode.tmpl");
    $tmpl->param(vm_codes => \@fast_vm_codes);
    return $tmpl->output;
}

my $data = read_code_data("gen/code.json");
my $inst_data = get_inst_data($data);

my $enum_code = gen_enum_code($inst_data);
open(ous, ">", "include/gen_vmcode.hpp.bak");
print ous $enum_code;
close(ous);

my $decl_code = gen_decl_code($inst_data);
open(ous, ">", "src/gen_decl_code.cpp.bak");
print ous $decl_code;
close(ous);

my $label_code = gen_label_code($inst_data);
open(ous, ">", "src/gen_label.cpp.bak");
print ous $label_code;
close(ous);

my $vm_code = gen_vm_run_code($inst_data);
open(ous, ">", "src/gen_vm.cpp.bak");
print ous $vm_code;
close(ous);

my $fast_vm_code = gen_fast_vm_code($inst_data);
open(ous, ">", "src/gen_fast_vmcode.cpp.bak");
print ous $fast_vm_code;
close(ous);
