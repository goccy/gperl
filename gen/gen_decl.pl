use JSON qw/encode_json decode_json/;
use Data::Dumper;
open(ins, "<gen/decl.json");
open(ous, ">include/gen_token.hpp");
my $json_data;
foreach (<ins>) {
	$json_data .= $_;
}
my $data = decode_json($json_data);
my @array = @{$data};
#print Dumper(@array);
my @token_enum = ();
for (my $i = 0; $i < $#array + 1; $i++) {
	my $type = $array[$i]->{type};
	unless (grep{$_ eq $type} @token_enum) {
		push(@token_enum, $type);
	}
}

print ous "typedef enum {\n";
foreach (@token_enum) {
	print ous "\t$_,\n";
}
print ous "} GPerlT;\n";
print ous "\n";
print ous
"typedef struct _GPerlTokenInfo {
	GPerlT type;
	const char *name;
	const char *data;
} GPerlTokenInfo;\n";

open(ous, ">src/gen_token_decl.cpp");
print ous "GPerlTokenInfo decl_tokens[] = {\n";
for (my $i = 0; $i < $#array + 1; $i++) {
	my $type = $array[$i]->{type};
	print ous "\t{$type, \"${type}\", \"$array[$i]->{data}\"},\n";
}
print ous "};\n";
print ous "\n";
