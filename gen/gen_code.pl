use JSON qw/encode_json decode_json/;
use Data::Dumper;

sub read_code_data {
    my $file_name = shift;
    open(ins, "<" . $file_name);
    my $json_data;
    foreach (<ins>) {
        $json_data .= $_;
    }
    my $data = decode_json($json_data);
    my @array = @{$data};
    return @array;
}

my @g_inst = ();
my @g_fast_inst = ();
sub get_inst_data {
    my $ref = shift;
    my @array = @{$ref};
    my @ret = ();
    foreach (@array) {
        my @inst_names = ();
        my @fast_inst_names = ();
        my $inst_name = $_->{code};
        my $type_flag = $_->{type};
        my $const_flag = $_->{const};
        my $fast_type = $_->{fast_type};
        my @args = $_->{args};

        my @fast_prefix;
        if ($fast_type == 1) {
            @fast_prefix = ("AB", "AC", "AD", "BC", "BD", "CD");
        } else {
            @fast_prefix = ("A", "B", "C", "D");
        }
        push(@inst_names, "${inst_name}");
        if ($fast_type) {
            foreach (@fast_prefix) {
                push(@fast_inst_names, "${_}_${inst_name}");
            }
        }
        if ($type_flag) {# gen Int, Double, String, Object, (TypeInference)
            push(@inst_names, "i${inst_name}");
            push(@inst_names, "f${inst_name}");
            push(@inst_names, "s${inst_name}");
            push(@inst_names, "o${inst_name}");
            if ($fast_type) {
                foreach (@fast_prefix) {
                    push(@fast_inst_names, "${_}_i${inst_name}");
                    push(@fast_inst_names, "${_}_f${inst_name}");
                    push(@fast_inst_names, "${_}_s${inst_name}");
                    push(@fast_inst_names, "${_}_o${inst_name}");
                }
            }
            if ($const_flag) {
                push(@inst_names, "i${inst_name}C");
                push(@inst_names, "f${inst_name}C");
                if ($fast_type) {
                    foreach (@fast_prefix) {
                        push(@fast_inst_names, "${_}_i${inst_name}C");
                        push(@fast_inst_names, "${_}_f${inst_name}C");
                        push(@fast_inst_names, "${_}_s${inst_name}C");
                        push(@fast_inst_names, "${_}_o${inst_name}C");
                    }
                }
            }
        } elsif ($const_flag) {
            push(@inst_names, "${inst_name}C");
            if ($fast_type) {
                foreach (@fast_prefix) {
                    push(@fast_inst_names, "${_}_${inst_name}C");
                }
            }
        }
        my %hash = (
            orig => $inst_name,
            inst_names => \@inst_names,
            fast_inst_names => \@fast_inst_names,
            args => \@args
            );
        push(@g_inst, @inst_names);
        push(@g_final_inst, @fast_inst_names);
        push(@ret, \%hash);
    }
    return @ret;
}

sub gen_enum_code {
    my $ret = "";
    my $ref = shift;
    my @insts = @{$ref};
    my @fast_inst_names = ();
    foreach my $inst (@insts) {
        my @inst_names = @{$inst->{inst_names}};
        my @_fast_inst_names = @{$inst->{fast_inst_names}};
        push(@fast_inst_names, @_fast_inst_names);
        foreach (@inst_names) {
            $ret .= "\t$_,\n";
        }
    }
    foreach (@fast_inst_names) {
        $ret .= "\t$_,\n";
    }
    $ret .= "} GPerlOpCode;\n";
    $ret .= "\n";
    return $ret;
}

sub gen_info_code {
    my $ret = "";
    $ret .=
"typedef struct _GPerlCodeInfo {
	GPerlOpCode code;
	const char *name;
} GPerlCodeInfo;\n\n";
    $ret .= "GPerlCodeInfo decl_codes[] = {\n";
    my $ref = shift;
    my @insts = @{$ref};
    my @fast_inst_names = ();
    foreach my $inst (@insts) {
        my @inst_names = @{$inst->{inst_names}};
        my @_fast_inst_names = @{$inst->{fast_inst_names}};
        push(@fast_inst_names, @_fast_inst_names);
        foreach (@inst_names) {
            $ret .= "\t{$_, \"${_}\"},\n";
        }
    }
    foreach (@fast_inst_names) {
        $ret .= "\t{$_, \"${_}\"},\n";
    }
    $ret .= "};\n";
    return $ret;
}

sub gen_inst_label_code {
    my $ret = "";
    my $jmp_label = "\tstatic void *jmp_table[] = {\n";
    my $ref = shift;
    my @insts = @{$ref};
    my @inst_names = ();
    my @fast_inst_names = ();
    foreach my $inst (@insts) {
        my @_inst_names = @{$inst->{inst_names}};
        my @_fast_inst_names = @{$inst->{fast_inst_names}};
        push(@inst_names, @_inst_names);
        push(@fast_inst_names, @_fast_inst_names);
    }
    for (my $i = 0; $i < $#inst_names; $i += 3) {
        my $inst = $inst_names[$i];
        my $inst2 = $inst_names[$i+1];
        my $inst3 = $inst_names[$i+2];
        $jmp_label .= "\t    &&L${inst}," if (defined($inst));
        $jmp_label .= " &&L${inst2}," if (defined($inst2));
        $jmp_label .= " &&L${inst3},\n" if (defined($inst3));
    }
    for (my $i = 0; $i < $#fast_inst_names; $i += 3) {
        my $inst = $fast_inst_names[$i];
        my $inst2 = $fast_inst_names[$i+1];
        my $inst3 = $fast_inst_names[$i+2];
        $jmp_label .= "\t    &&L${inst}," if (defined($inst));
        $jmp_label .= " &&L${inst2}," if (defined($inst2));
        $jmp_label .= " &&L${inst3},\n" if (defined($inst3));
    }
    $jmp_label .= "\n\t};\n";
    my $block_label .= "\tstatic InstBlock block_table[] = {\n";
    for (my $i = 0; $i < $#inst_names; $i += 3) {
        my $inst = $inst_names[$i];
        my $inst2 = $inst_names[$i+1];
        my $inst3 = $inst_names[$i+2];
        $block_label .= "\t    B(${inst})," if (defined($inst));
        $block_label .= " B(${inst2})," if (defined($inst2));
        $block_label .= " B(${inst3}),\n" if (defined($inst3));
    }
    for (my $i = 0; $i < $#fast_inst_names; $i += 3) {
        my $inst = $fast_inst_names[$i];
        my $inst2 = $fast_inst_names[$i+1];
        my $inst3 = $fast_inst_names[$i+2];
        $block_label .= "\t    B(${inst})," if (defined($inst));
        $block_label .= " B(${inst2})," if (defined($inst2));
        $block_label .= " B(${inst3}),\n" if (defined($inst3));
    }
    $block_label .= "\n\t};\n";
    $ret = $jmp_label . $block_label;
    return $ret;
}

sub gen_inst_body {
    my $ret = "";
    my $ref = shift;
    my @insts = @{$ref};
    foreach my $inst (@insts) {
        my @inst_names = @{$inst->{inst_names}};
        my @args = @{$inst->{args}};
        foreach (@inst_names) {
            my $op = "";
            if ($_ =~ "ADD") {
                $op .= "+=";
            } elsif ($_ =~ /SUB/) {
                $op .= "-=";
            } elsif ($_ =~ /MUL/) {
                $op .= "*=";
            } elsif ($_ =~ /DIV/) {
                $op .= "/=";
            } elsif ($_ =~ /MOV/) {
                $op .= "=";
            }
            my $decl_args = "";
            foreach (@args) {
                my @a = @{$_};
                my $isFirst = 1;
                foreach (@a) {
                    my %hash = %{$_};
                    my @keys = keys(%hash);
                    if ($isFirst) {
                        $decl_args .= $keys[0];
                        $isFirst = 0;
                    } else {
                        $decl_args .= ", " . $keys[0];
                    }
                }
            }
            my $tmp = "#define GPERL_${_}(" . $decl_args . ") ";
            if ($_ =~ /^i/) {
                $tmp .= "I(data)[dst] " . $op . " I(data)[src]\n";
            } elsif ($_ =~ /^d/) {
                $tmp .= "D(data)[dst] " . $op . " D(data)[src]\n";
            } elsif ($_ =~ /^s/) {
            } elsif ($_ =~ /^o/) {
            } else {
            }
            $ret .= $tmp . "\n";
        }
    }
    return $ret;
}

my @array = read_code_data("gen/code.json");
open(ous, ">include/gen_vmcode.hpp");
my @insts = get_inst_data(\@array);
my $enum_code = gen_enum_code(\@insts);
my $info_code = gen_info_code(\@insts);
print ous $enum_code;
print ous $info_code;
# =============== DECLARE LABEL =================
open(ous, ">src/gen_vm.cpp");
my $label_code = gen_inst_label_code(\@insts);
print gen_inst_body(\@insts);

print ous $label_code;
for (my $i = 0; $i < $#hash_array; $i++) {
	my %hash = %{$hash_array[$i]};
	my @inst = @{$hash{inst}};
	foreach (@inst) {
        print ous "#define GPERL_${_}(dst, src) ";
        if ($_ =~ /^i/) {
            print ous "I(data)[dst] ";
        } elsif ($_ =~ /^f/) {
            print ous "F(data)[dst] ";
        } elsif ($_ =~ /^s/) {
            print ous "S(data)[dst] ";
        } elsif ($_ =~ /^o/) {
            print ous "O(data)[dst] ";
        } else {
            print ous "O(data)[dst] ";
        }
        if ($_ =~ "ADD") {
            print ous "+= ";
        } elsif ($_ =~ /SUB/) {
            print ous "-= ";
        } elsif ($_ =~ /MUL/) {
            print ous "*= ";
        } elsif ($_ =~ /DIV/) {
            print ous "/= ";
        } elsif ($_ =~ /MOV/) {
            print ous "= ";
        }
        if ($_ =~ /C$/) {
            print ous "src\n";
        } elsif ($_ =~ /^i/) {
            print ous "I(data)[src]\n";
        } elsif ($_ =~ /^f/) {
            print ous "F(data)[src]\n";
        } elsif ($_ =~ /^s/) {
            print ous "S(data)[src]\n";
        } elsif ($_ =~ /^o/) {
            print ous "O(data)[src]\n";
        } else {
            print ous "O(data)[src] //NaN-boxing\n";
        }
    }
}

print ous
"\nint GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes;
	Reg reg_[MAX_CALLSTACK_SIZE];
	Reg *reg = reg_;
	GPerlEnv *callstack = createCallStack();
	GPerlObject **argstack = createArgStack();
	static char shared_buf[128] = {0};//TODO must be variable buffer
	static string outbuf = \"\";

#include \"gen_label.cpp\"

    DISPATCH_START();

";
for (my $i = 0; $i < $#hash_array; $i++) {
	my %hash = %{$hash_array[$i]};
	my $code = $hash{code};
	my @inst = @{$hash{inst}};
	foreach (@inst) {
		print ous "\tCASE(${_}), {\n";
		print ous "\t\tGPERL_${_}();\n";
		print ous "\t\tpc++;\n";
		print ous "\t\tBREAK();\n";
		print ous "\t});\n";
	}
}
print ous "#include \"gen_fast_vmcode.cpp\"\n\n";
print ous "\tDISPATCH_END();\n";
print ous "\treturn I(data)[0];\n";
print ous "}\n";

open(ous, ">src/gen_fast_vmcode.cpp");
print ous "/*****************************************/\n";
print ous "/*           FAST INSTRUCTION            */\n";
print ous "/*****************************************/\n";
for (my $i = 0; $i < $#hash_array; $i++) {
	my %hash = %{$hash_array[$i]};
	my $code = $hash{code};
	my @final_inst = @{$hash{final_inst}};
	foreach (@final_inst) {
        my @_code = split("_", $_);
		print ous "\tCASE(${_}), {\n";
		print ous "\t\tGPERL_${_code[1]}();\n";
		print ous "\t\tpc++;\n";
		print ous "\t\tBREAK();\n";
		print ous "\t});\n";
	}
}
