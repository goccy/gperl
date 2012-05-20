use JSON qw/encode_json decode_json/;
use Data::Dumper;

open(ins, "<gen/code.json");
open(ous, ">include/gen_vmcode.hpp");
my $json_data;
foreach (<ins>) {
	$json_data .= $_;
}
my $data = decode_json($json_data);
my @array = @{$data};


my @hash_array = ();
my @g_inst = ();
my @g_final_inst = ();
foreach (@array) {
	my @inst = ();
	my @final_inst = ();
	my $code = $_->{code};
	my $type = $_->{type};
	my $const = $_->{const};
	my $final = $_->{final};
	my @prefix;
	if ($final == 1) {
		@prefix = ("AB", "AC", "AD", "BC", "BD", "CD");
	} else {
		@prefix = ("A", "B", "C", "D");
	}
	push(@inst, "${code}");
	if ($final) {
		foreach (@prefix) {
			push(@final_inst, "${_}_${code}");
		}
	}
	if ($type) {
		push(@inst, "i${code}");
		push(@inst, "f${code}");
		push(@inst, "s${code}");
		push(@inst, "o${code}");
		if ($final) {
			foreach (@prefix) {
				push(@final_inst, "${_}_i$code");
				push(@final_inst, "${_}_f$code");
				push(@final_inst, "${_}_s$code");
				push(@final_inst, "${_}_o$code");
			}
		}
		if ($const) {
			push(@inst, "i${code}C");
			push(@inst, "f${code}C");
			push(@inst, "s${code}C");
			push(@inst, "o${code}C");
			if ($final) {
				foreach (@prefix) {
					push(@final_inst, "${_}_i${code}C");
					push(@final_inst, "${_}_f${code}C");
					push(@final_inst, "${_}_s${code}C");
					push(@final_inst, "${_}_o${code}C");
				}
			}
		}
	} elsif ($const) {
		push(@inst, "${code}C");
		if ($final) {
			foreach (@prefix) {
				push(@final_inst, "${_}_$codeC");
			}
		}
	}
	my %inst_hash = (
		code => $code,
		inst => \@inst,
		final_inst => \@final_inst
	);
	push(@g_inst, @inst);
	push(@g_final_inst, @final_inst);
	push(@hash_array, \%inst_hash);
}

print ous "typedef enum {\n";
foreach (@g_inst) {
	print ous "\t$_,\n";
}
foreach (@g_final_inst) {
	print ous "\t$_,\n";
}
print ous "} GPerlOpCode;\n";
print ous "\n";
print ous
"typedef struct _GPerlCodeInfo {
	GPerlOpCode code;
	const char *name;
} GPerlCodeInfo;\n";
print ous "\n";

print ous "GPerlCodeInfo decl_codes[] = {\n";
foreach (@g_inst) {
	print ous "\t{$_, \"${_}\"},\n";
}
foreach (@g_final_inst) {
	print ous "\t{$_, \"${_}\"},\n";
}
print ous "};\n";

open(ous, ">src/gen_vm.cpp");

for (my $i = 0; $i < $#hash_array; $i++) {
	my %hash = %{$hash_array[$i]};
	my $code = $hash{code};
	print ous "#define GPERL_${code}() \n";
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
\n";
print ous "\tstatic void *jmp_table[] = {\n";
for (my $i = 0; $i < $#g_inst; $i += 3) {
	my $inst = $g_inst[$i];
	my $inst2 = $g_inst[$i+1];
	my $inst3 = $g_inst[$i+2];
	print ous "\t    &&L${inst}," if (defined($inst));
	print ous " &&L${inst2}," if (defined($inst2));
	print ous " &&L${inst3},\n" if (defined($inst3));
}

for (my $i = 0; $i < $#g_final_inst; $i += 3) {
	my $inst = $g_final_inst[$i];
	my $inst2 = $g_final_inst[$i+1];
	my $inst3 = $g_final_inst[$i+2];
	print ous "\t    &&L${inst}," if (defined($inst));
	print ous " &&L${inst2}," if (defined($inst2));
	print ous " &&L${inst3},\n" if (defined($inst3));
}
print ous "\n";
print ous "\t};\n";

print ous "\tstatic InstBlock block_table[] = {\n";

for (my $i = 0; $i < $#g_inst; $i += 3) {
	my $inst = $g_inst[$i];
	my $inst2 = $g_inst[$i+1];
	my $inst3 = $g_inst[$i+2];
	print ous "\t    B(${inst})," if (defined($inst));
	print ous " B(${inst2})," if (defined($inst2));
	print ous " B(${inst3}),\n" if (defined($inst3));
}

for (my $i = 0; $i < $#g_final_inst; $i += 3) {
	my $inst = $g_final_inst[$i];
	my $inst2 = $g_final_inst[$i+1];
	my $inst3 = $g_final_inst[$i+2];
	print ous "\t    B(${inst})," if (defined($inst));
	print ous " B(${inst2})," if (defined($inst2));
	print ous " B(${inst3}),\n" if (defined($inst3));
}
print ous "\n";
print ous "\t};\n";

print ous "\tDISPATCH_START();\n";
for (my $i = 0; $i < $#hash_array; $i++) {
	my %hash = %{$hash_array[$i]};
	my $code = $hash{code};
	my @inst = @{$hash{inst}};
	foreach (@inst) {
		print ous "\tCASE(${_}), {\n";
		print ous "\t\tGPERL_${code}();\n";
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
		print ous "\tCASE(${_}), {\n";
		print ous "\t\tGPERL_${code}();\n";
		print ous "\t\tpc++;\n";
		print ous "\t\tBREAK();\n";
		print ous "\t});\n";
	}
}
