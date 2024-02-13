#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Std;

my %opts;
getopts('i:o:', \%opts);

my $input_file = \*STDIN;
my $output_file = \*STDOUT;

if ($opts{i}) {
  open($input_file, "<", $opts{i});
}

if ($opts{o}) {
  open($output_file, ">", $opts{o});
}

my @alloc_directives;
my @bypass_lines;
my @declarations;
my @allocfns;
my @reallocfns;
my @dumpfns;
my $hash_function_name;
my $hash_function;



=head1 NAME

AllocPP.pl - Perl script for generating static allocation heaps in C

=head1 SYNOPSIS

    AllocPP.pl [-i input_file] [-o output_file]

=head1 DESCRIPTION

This script scans a C file for specific directives and generates static allocation heaps based on those directives.

The directives are in the style of C preprocessor directives. These directives, and their signature, are as follows:

	#alloc <heap name> , <allocate function name> , <reallocate function name> , <dump function name>
	#hashfunc <hash function name>

The signature of the generated functions is:

	void* <allocate function>(size_t size);
	void* <reallocate function>(void* ptr, size_t resize);
	void  <dump function>(void);

All the directive parameters are mandatory. You can create several heaps, but only one '#hashfunc' directive is necessary.

Basically, the #alloc directive creates an allocation heap, and an allocation, reallocation, and dump function for it. You can use this heap to allocate and reallocate data, and then dump it at the end.

The '#hashfunc' directive names the MurMurHash function that is necessary for the process.

Notice that the functions do safe allocations and reallocations; you do not need to check for errors and null pointers. If a returned pointer for calloc and realloc is NULL, it will error out and exit.

Also, if the size given to realloc is smaller than the already-given size, it will warn the user.

=head1 EXAMPLE

Suppose you have a C file named "example.c" with the following content:

    #include <stdio.h>
    #include <stdlib.h>

    // Your existing C code here...

    #alloc heap1, allocate1, reallocate1, dump1
    #alloc heap2, allocate2, reallocate2, dump2
    #hashfunc my_hash_function

    int main() {
        void* int ptr1 = allocate1(sizeof(int));
        void* int ptr2 = allocate2(sizeof(int));

        *ptr1 = 22;
        *ptr2 = 33;

        ptr1 = reallocate1(ptr1, sizeof(int) * 2);
        *(ptr1 + sizeof(int)) = 44;

        // do stuff

        dump1();
        dump2();
    }
 
=head1 OPTIONS

=over 4

=item B<-i>

Input file. If not provided, it defaults to STDIN.

=item B<-o>

Output file. If not provided, it defaults to STDOUT.

=head1 AUTHOR

This script was written by Chubak Bidpaa (chubakbidpaa@gmail.com).

=head1 COPYRIGHT

GNU GENERAL PUBLIC LICENSE
Version 3, June 2007

Preamble:
The GNU General Public License is a free, copyleft license for software and other kinds of works.

Terms and Conditions:
- You are free to run, modify, and distribute the software.
- Source code access is required for any distributed modifications.
- Derivative works must be distributed under the GPL.

Additional Permissions:
- Propagation of covered works is permitted without modification.
- Adding exceptions to this license is allowed.

Disclaimer of Warranty:
The software is provided without any warranty.

Full License Text:
The complete text of the GNU General Public License version 3.0 can be found at [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html).

=back

=cut

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
		node->alloc_size = resize;
            }
            return alloc_error_check;
        }
    }

    // in case the node does not exist, we alloate it
    return $alloc_directive->{allocfn_name}(resize);
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


close($input_file) if defined $opts{i};
close($output_file) if defined $opts{o};
