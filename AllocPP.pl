#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Std;

my %opts;
getopts('i:o:', \%opts);

my $input_file = $opts{i} || \*STDIN;
my $output_file = $opts{o} || \*STDOUT;

my @alloc_directives;
my @bypass_lines;
my @declarations;
my @allocfns;
my @reallocfns;
my @dumpfns;
my $hash_function_name;
my $hash_function;

while (<$input_file>) {
    if (/^#alloc\s+(.+)\s*,\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*,\s*([a-zA-Z_][a-zA-Z0-9_]*),\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*$/) {
        push @alloc_directives, {
            heap_name       => $1,
            allocfn_name    => $2,
            reallocfn_name  => $3,
            dumpfn_name     => $4,
        };
    } elsif (/^#hashfunc\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*$/) {
        $hash_function_name = $1;
    } else {
        push @bypass_lines, $_;
    }
}

sub generate_pointer_hash_function {
    my $random_integer = int(rand(100)) + 1; 
    $hash_function = <<"END_HASH_FUNCTION";
unsigned long long $hash_function_name(void *ptr) {
    const unsigned long long m = 0xc6a4a7935bd1e995;
    const int r = 47;
    unsigned long long h = 0 ^ (sizeof(unsigned long long) * 8);
    unsigned long long k = (unsigned long long)ptr;
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h + $random_integer;
}
END_HASH_FUNCTION
}

sub generate_declarations {
    my ($alloc_directives_ref) = @_;

    push @declarations, "unsigned long long $hash_function_name(void *ptr);";
    push @declarations, "#define ALLOCPP_NULL ((void *)0)";

    foreach my $alloc_directive (@$alloc_directives_ref) {
        my $heap_decl = <<"END_HEAP";
// Declaration of the $alloc_directive->{heap_name} heap
 typedef struct $alloc_directive->{heap_name} {
     unsigned long long alloc_size;
     void *alloc_data;
     unsigned long long hash;
     struct $alloc_directive->{heap_name} *next;
} $alloc_directive->{heap_name};
END_HEAP
        push @declarations, $heap_decl;
        push @declarations, "void *$alloc_directive->{allocfn_name}(unsigned long long size);";
        push @declarations, "void *$alloc_directive->{reallocfn_name}(void* ptr, unsigned long long size);";
        push @declarations, "void $alloc_directive->{dumpfn_name}(void);";
        push @declarations, "$alloc_directive->{heap_name} *___HEAP__$alloc_directive->{heap_name} = ALLOCPP_NULL;";
    }
}

sub generate_allocfns {
    my ($alloc_directives_ref) = @_;

    foreach my $alloc_directive (@$alloc_directives_ref) {
        my $allocfn_decl = <<"END_FUNC";
// Definition of the $alloc_directive->{allocfn_name} allocation function
void *$alloc_directive->{allocfn_name}(unsigned long long size) {
    $alloc_directive->{heap_name} *node = calloc(1, sizeof($alloc_directive->{heap_name}));
    node->next = ___HEAP__$alloc_directive->{heap_name};
    void *alloc_error_check = calloc(1, size);
    if (alloc_error_check == NULL) {
	fprintf(stderr, "Allocation error");
	fputc(10, stderr);
    }
    node->alloc_data = calloc(1, size);
    node->alloc_size = size;
    node->hash = $hash_function_name(node->alloc_data);
    ___HEAP__$alloc_directive->{heap_name} = node;
    return node->alloc_data;
}
END_FUNC
        push @allocfns, $allocfn_decl;
    }
}

sub generate_reallocfns {
    my ($alloc_directives_ref) = @_;

    foreach my $alloc_directive (@$alloc_directives_ref) {
        my $reallofn_decl = <<"END_FUNC";
// Definition of the $alloc_directive->{reallocfn_name} reallocation function
void *$alloc_directive->{reallocfn_name}(void *ptr, unsigned long long resize) {
    $alloc_directive->{heap_name} *node = ___HEAP__$alloc_directive->{heap_name};
    unsigned long long hash = $hash_function_name(ptr);
    for (; node != NULL; node = node->next) {
        if (node->hash == hash && node->alloc_data == ptr) {
            if (resize <= node->alloc_size) {
                fprintf(stderr, "Reallocation warning!");
                fputc(10, stderr);
            }
            void *alloc_error_check = realloc(node->alloc_data, resize);
            if (alloc_error_check == NULL) {
                fprintf(stderr, "Reallocation error");
                fputc(10, stderr);
                exit(EXIT_FAILURE);
            } else {
                node->alloc_data = alloc_error_check;
            }
            return alloc_error_check;
        }
    }

    fprintf(stderr, "Error: the pointer does not belong to this heap");
    fputc(10, stderr);
}
END_FUNC
        push @reallocfns, $reallofn_decl;
    }
}

sub generate_dumpfns {
    my ($alloc_directives_ref) = @_;

    foreach my $alloc_directive (@$alloc_directives_ref) {
        my $dumpfn_decl = <<"END_FUNC";
void $alloc_directive->{dumpfn_name}(void) {
// definition of the $alloc_directive->{heap_name} dump heap function
    $alloc_directive->{heap_name} *node = ___HEAP__$alloc_directive->{heap_name};
    while (node != NULL) {
        free(node->alloc_data);
        $alloc_directive->{heap_name} *temp = node;
        node = node->next;
        free(temp);
    }
}
END_FUNC
        push @dumpfns, $dumpfn_decl;
    }
}

generate_declarations(\@alloc_directives);
generate_allocfns(\@alloc_directives);
generate_reallocfns(\@alloc_directives);
generate_dumpfns(\@alloc_directives);
generate_pointer_hash_function();

# Print declarations
print $output_file "/* Declarations, generated by AllocPP.pl */\n\n";
print $output_file join("\n\n", @declarations), "\n\n";

# Print normal lines
print $output_file "/* The main text of the program, untouched */\n\n";
print $output_file join("", @bypass_lines), "\n\n";

# Print definitions
print $output_file "/* The allocate functions, generated by AllocPP.pl */\n\n";
print $output_file join("\n", @allocfns), "\n\n";
print $output_file "/* The reallocate functions, generated by AllocPP.pl */\n\n";
print $output_file join("\n", @reallocfns), "\n\n";
print $output_file "/* The dump functions, generated by AllocPP.pl */\n\n";
print $output_file join("\n", @dumpfns), "\n\n\n";
print $output_file "/* The hash function, generated by AllocPP.pl */\n\n";
print $output_file $hash_function, "\n\n";

# Print notifications
print $output_file "\n", "/* The preceding file was preprocessed with AllocPP.pl, a Perl script for generating static allocation heaps in C */", "\n\n";

