#!/usr/bin/perl

use strict;
use warnings;

sub ln80 {
	my $file = $_[0];
	$/ = "\n";
	open(FILE, $_[0])
		or die("$file: no such file or direcory\n");
	while (<FILE>) {
		chomp;
		1 while $_ =~ s/\t+/' ' x (length($&) * 8 - length($`) % 8)/e;
		if (length($_) > 80) {
			print "WRN: in $file line $. has ".length($_)."/80 chars\n";
		}
	}
	close (FILE);
}

sub eolSpace {
	my $file = $_[0];
	$/ = "\n";
	open(FILE, $_[0])
		or die("$file: no such file or direcory\n");
	while (<FILE>) {
		chomp;
		if ($_ =~ /\s+$/) {
			print "WRN: in $file line $. has trailing whitespaces\n";
		}
	}
	close (FILE);
}

if (not $ARGV[0]) {
	open(LS, 'find -name \*.\[c\|h\] |');
	while (<LS>) {
		chomp;
		my $string = $_;
		eolSpace($string);
		ln80($string);
	}
} else {
	local $/ = " ";
	foreach my $arg(@ARGV) {
		ln80($arg);
		eolSpace($arg);
	}
}
