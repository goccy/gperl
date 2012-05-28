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
        if ($type_flag) {# gen Int, Double, String, Object, (TypeInference)
            push(@inst_names, "d${inst_name}");
            push(@inst_names, "i${inst_name}");
            if ($inst_name !~ /SUB/ && $inst_name !~ /DIV/) {
                push(@inst_names, "s${inst_name}");
            }
            push(@inst_names, "o${inst_name}");
            if ($fast_type) {
				foreach (@fast_prefix) {
					push(@fast_inst_names, "${_}_${inst_name}");
					push(@fast_inst_names, "${_}_d${inst_name}");
					push(@fast_inst_names, "${_}_i${inst_name}");
                    if ($inst_name !~ /SUB/ && $inst_name !~ /DIV/) {
                        push(@fast_inst_names, "${_}_s${inst_name}");
                    }
                    push(@fast_inst_names, "${_}_o${inst_name}");
				}
            }
            if ($const_flag) {
                push(@inst_names, "d${inst_name}C");
                push(@inst_names, "i${inst_name}C");
                if ($fast_type) {
					@fast_prefix = ("A", "B", "C", "D");
					foreach (@fast_prefix) {
						push(@fast_inst_names, "${_}_${inst_name}C");
						push(@fast_inst_names, "${_}_d${inst_name}C");
						push(@fast_inst_names, "${_}_i${inst_name}C");
                        if ($inst_name !~ /SUB/ && $inst_name !~ /DIV/) {
                            push(@fast_inst_names, "${_}_s${inst_name}C");
                        }
                        if ($inst_name !~ /ADD/ &&
                            $inst_name !~ /SUB/ &&
                            $inst_name !~ /MUL/ &&
                            $inst_name !~ /DIV/) {
                            push(@fast_inst_names, "${_}_o${inst_name}C");
                        }
					}
                }
            }
        } elsif ($const_flag) {
            push(@inst_names, "${inst_name}C");
            if ($fast_type) {
				@fast_prefix = ("A", "B", "C", "D");
                foreach (@fast_prefix) {
                    push(@fast_inst_names, "${_}_${inst_name}C");
                }
            }
        } else {
            if ($fast_type) {
				foreach (@fast_prefix) {
					push(@fast_inst_names, "${_}_${inst_name}");
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
    my $ret = "typedef enum {\n";
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
    $ret .=
"typedef struct _GPerlCodeInfo {
	GPerlOpCode code;
	const char *name;
} GPerlCodeInfo;\n\n";
    return $ret;
}

sub gen_info_code {
    my $ret = "";
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
    for (my $i = 0; $i <= $#inst_names; $i += 3) {
        my $inst = $inst_names[$i];
        my $inst2 = $inst_names[$i+1];
        my $inst3 = $inst_names[$i+2];
        $jmp_label .= "\t    &&L(${inst})," if (defined($inst));
        $jmp_label .= " &&L(${inst2})," if (defined($inst2));
        $jmp_label .= " &&L(${inst3}),\n" if (defined($inst3));
    }
    for (my $i = 0; $i <= $#fast_inst_names; $i += 3) {
        my $inst = $fast_inst_names[$i];
        my $inst2 = $fast_inst_names[$i+1];
        my $inst3 = $fast_inst_names[$i+2];
        $jmp_label .= "\t    &&L(${inst})," if (defined($inst));
        $jmp_label .= " &&L(${inst2})," if (defined($inst2));
        $jmp_label .= " &&L(${inst3}),\n" if (defined($inst3));
    }
    $jmp_label .= "\n\t};\n";
    my $block_label .= "\tstatic InstBlock block_table[] = {\n";
    for (my $i = 0; $i <= $#inst_names; $i += 3) {
        my $inst = $inst_names[$i];
        my $inst2 = $inst_names[$i+1];
        my $inst3 = $inst_names[$i+2];
        $block_label .= "\t    B(${inst})," if (defined($inst));
        $block_label .= " B(${inst2})," if (defined($inst2));
        $block_label .= " B(${inst3}),\n" if (defined($inst3));
    }
    for (my $i = 0; $i <= $#fast_inst_names; $i += 3) {
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
		my $cmp_flag = 0;
		my $ret_flag = 0;

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
            } elsif ($_ =~ /J/ && $_ !~ /JMP/) {
				$cmp_flag = 1;
			} elsif ($_ =~ /RET/) {
				$ret_flag = 1;
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
			if ($cmp_flag) {
				next;
			}
            my $tmp = "#define GPERL_${_}(" . $decl_args . ") ";
            if ($_ =~ /^i/) {
				if ($ret_flag) {
					$tmp .= "return I(data)[src]\n";
				} else {
					$tmp .= "I(data)[dst] " . $op . " I(data)[src]\n";
				}
            } elsif ($_ =~ /^d/) {
				if ($ret_flag) {
					$tmp .= "return D(data)[src]\n";
				} else {
					$tmp .= "D(data)[dst] " . $op . " D(data)[src]\n";
				}
            } elsif ($_ =~ /^s/) {
				if ($ret_flag) {
					$tmp .= "return S(data)[src]\n";
				} else {
					$tmp .= "S(data)[dst] " . $op . " S(data)[src]\n";
				}
            } elsif ($_ =~ /^o/) {
				$tmp .= "\n";
            } else {
				$tmp .= "\n";
            }
            $ret .= $tmp;
        }
    }
    return $ret;
}

my $run_init = "
#include <gperl.hpp>
#include \"gen_decl_code.cpp\"
#include <vmlibs.hpp>

using namespace std;

int GPerlVirtualMachine::run(GPerlVirtualMachineCode *codes)
{
	static GPerlVirtualMachineCode *top;
	GPerlVirtualMachineCode *pc = codes;
	GPerlEnv *callstack = createCallStack();
	GPerlObject **argstack = createArgStack();
	static char shared_buf[128] = {0};//TODO must be variable buffer
	static string outbuf = \"\";

#include \"gen_label.cpp\"

    DISPATCH_START();

";

my $type_check_code =
"		int dst_type = TYPE_CHECK(callstack->reg[pc->dst]);
		int src_type = TYPE_CHECK(callstack->reg[pc->src]);
#ifdef STATIC_TYPING_MODE
		pc->opnext = jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#else /* DYNAMIC_TYPING_MODE */
		goto *jmp_table[pc->op + 1 + ((dst_type + src_type) >> 1)];
#endif
";
sub gen_vm_run_code {
	my $ret = $run_init;
    my $ref = shift;
    my @insts = @{$ref};
    my @inst_names = ();
    foreach my $inst (@insts) {
        my @inst_names = @{$inst->{inst_names}};
		my @args = @{$inst->{args}};
		foreach (@inst_names) {
			my $orig = $_;
			$ret .= "\tCASE(${_}, {\n";
			my $decl_args = "";
			foreach (@args) {
				my @a = @{$_};
				my $isFirst = 1;
				foreach (@a) {
					my %hash = %{$_};
					my @values = values(%hash);
					if ($isFirst) {
						$decl_args .= $values[0];
						$isFirst = 0;
					} elsif ($values[0] =~ /callname/) {
						$decl_args .= ", " . $orig;
					} else {
						$decl_args .= ", " . $values[0];
					}
				}
			}
			if ($_ =~ /J/ && $_ !~ /JMP/) {
				my $prefix = substr($_, 0, 1);
				$prefix = "" if ($prefix =~ /J/);
				if ($_ =~ /C$/) {
					$ret .= "\t\tGPERL_${prefix}CMP_JMPC(" . $decl_args . ");\n";
				} elsif($prefix eq "") {
					$ret .= $type_check_code;
				} else {
					$ret .= "\t\tGPERL_${prefix}CMP_JMP(" . $decl_args . ");\n";
				}
			} elsif ($_ =~ /THCODE/) {
			} elsif ($_ eq "ADD" || $_ eq "SUB" ||
					 $_ eq "MUL" || $_ eq "DIV") {
				$ret .= $type_check_code;
			} else {
				$ret .= "\t\tGPERL_${_}(" . $decl_args . ");\n";
				$ret .= "\t\tpc++;\n";
			}
			if ($_ =~ /THCODE/) {
				$ret .= "\t\tcreateDirectThreadingCode(codes, jmp_table);\n";
				$ret .= "\t\t(void)block_table;\n";
				$ret .= "\t\t//createSelectiveInliningCode(codes, jmp_table, block_table);\n";
				$ret .= "\t\treturn 0;\n";
			} else {
				$ret .= "\t\tBREAK();\n";
			}
			$ret .= "\t});\n";
		}
	}
	$ret .= "#include \"gen_fast_vmcode.cpp\"\n\n";
	$ret .= "\tDISPATCH_END();\n";
	$ret .= "\treturn I(0);\n";
	$ret .= "}\n";
	return $ret;
}

sub gen_fast_vm_code {
	my $ret = "";
    my $ref = shift;
    my @insts = @{$ref};
    my @inst_names = ();
    foreach my $inst (@insts) {
        my @inst_names = @{$inst->{fast_inst_names}};
		my @args = @{$inst->{args}};
		foreach (@inst_names) {
			my $orig = $_;
			$ret .= "\tCASE(${_}, {\n";
			my @_code = split("_", $_);
			my $fast_prefix = substr($_code[0], 0, 1);
			my $fast_prefix2 = substr($_code[0], 1, 1);
			my $decl_args = "";
			foreach (@args) {
				my @a = @{$_};
				my $isFirst = 1;
				foreach (@a) {
					my %hash = %{$_};
					my @values = values(%hash);
					if ($isFirst) {
						if ($values[0] =~ /dst/) {
							if ($fast_prefix =~ "A") {
								$decl_args .= "0";
							} elsif ($fast_prefix =~ "B") {
								$decl_args .= "1";
							} elsif ($fast_prefix =~ "C") {
								$decl_args .= "2";
							} elsif ($fast_prefix =~ "D") {
								$decl_args .= "3";
							}
						} elsif ($values[0] =~ /src/) {
							if ($fast_prefix2 =~ "A") {
								$decl_args .= "0";
							} elsif ($fast_prefix2 =~ "B") {
								$decl_args .= "1";
							} elsif ($fast_prefix2 =~ "C") {
								$decl_args .= "2";
							} elsif ($fast_prefix2 =~ "D") {
								$decl_args .= "3";
							} elsif (($orig =~ /RET/ || $orig =~ /SHIFT/) &&
									 $fast_prefix =~ "A") {
								$decl_args .= "0";
							} elsif (($orig =~ /RET/ || $orig =~ /SHIFT/) &&
									 $fast_prefix =~ "B") {
								$decl_args .= "1";
							} elsif (($orig =~ /RET/ || $orig =~ /SHIFT/) &&
									 $fast_prefix =~ "C") {
								$decl_args .= "2";
							} elsif (($orig =~ /RET/ || $orig =~ /SHIFT/) &&
								$fast_prefix =~ "D") {
								$decl_args .= "3";
							}
						} else {
							$decl_args .= $values[0];
						}
						$isFirst = 0;
					} else {
						if ($values[0] =~ /dst/) {
							if ($fast_prefix =~ "A") {
								$decl_args .= ", 0";
							} elsif ($fast_prefix =~ "B") {
								$decl_args .= ", 1";
							} elsif ($fast_prefix =~ "C") {
								$decl_args .= ", 2";
							} elsif ($fast_prefix =~ "D") {
								$decl_args .= ", 3";
							}
						} elsif ($values[0] =~ /src/) {
							if ($fast_prefix2 =~ "A") {
								$decl_args .= ", 0";
							} elsif ($fast_prefix2 =~ "B") {
								$decl_args .= ", 1";
							} elsif ($fast_prefix2 =~ "C") {
								$decl_args .= ", 2";
							} elsif ($fast_prefix2 =~ "D") {
								$decl_args .= ", 3";
							} else {
								$decl_args .= ", " . $values[0];
							}
						} elsif ($values[0] =~ /callname/) {
							$decl_args .= ", " . $orig;
						} else {
							$decl_args .= ", " . $values[0];
						}
					}
				}
			}
			if ($_ =~ /J/ && $_ !~ /JMP/) {
				my $prefix = substr($_code[1], 0, 1);
				$prefix = "" if ($prefix =~ /J/);
				if ($_ =~ /C$/) {
					$ret .= "\t\tGPERL_${prefix}CMP_JMPC(" . $decl_args . ");\n";
				} elsif($prefix eq "") {
					$ret .= $type_check_code;
				} else {
					$ret .= "\t\tGPERL_${prefix}CMP_JMP(" . $decl_args . ");\n";
				}
			} elsif ($_code[1] eq "ADD" || $_code[1] eq "SUB" ||
					 $_code[1] eq "MUL" || $_code[1] eq "DIV") {
				$ret .= $type_check_code;
			} else {
				$ret .= "\t\tGPERL_${_code[1]}(" . $decl_args . ");\n";
				$ret .= "\t\tpc++;\n";
			}
			$ret .= "\t\tBREAK();\n";
			$ret .= "\t});\n";
		}
	}
	return $ret;
}

my @array = read_code_data("gen/code.json");
my @insts = get_inst_data(\@array);

open(ous, ">include/gen_vmcode.hpp");
my $enum_code = gen_enum_code(\@insts);
print ous $enum_code;

open(ous, ">src/gen_decl_code.cpp");
my $info_code = gen_info_code(\@insts);
print ous $info_code;

open(ous, ">src/gen_label.cpp");
my $label_code = gen_inst_label_code(\@insts);
print ous $label_code;

open(ous, ">src/gen_vm.cpp");
#my $body = gen_inst_body(\@insts);
my $vm_code = gen_vm_run_code(\@insts);
#print ous $body;
print ous $vm_code;
my $fast_vm_code = gen_fast_vm_code(\@insts);
open(ous, ">src/gen_fast_vmcode.cpp");
print ous $fast_vm_code;
