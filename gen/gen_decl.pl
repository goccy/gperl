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
my @token_enum = ();
my @kind_enum = ();
foreach my $elem (@array) {
	my $type = $elem->{type};
	my $kind = $elem->{type};
	unless (grep{$_ eq $type} @token_enum) {
		push(@token_enum, $type);
	}
	unless (grep{$_ eq $kind} @kind_enum) {
		push(@kind_enum, $kind);
	}
}

print ous "typedef enum {\n";
foreach (@token_enum) {
	print ous "\t$_,\n";
}
print ous "} GPerlT;\n";
print ous "\n";

print ous "typedef enum {\n";
foreach (@kind_enum) {
	print ous "\t$_,\n";
}
print ous "} GPerlKind;\n";
print ous "\n";

print ous "
typedef struct _GPerlTokenInfo {
	GPerlT type;
	GPerlKind kind;
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
