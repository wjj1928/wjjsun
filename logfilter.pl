#!/bin/perl
#
# VER     Description
# 1.0     Initial version Add log filter 
#
use warnings;
use strict;

my $regexp;
my $pattern;
my $input_line;
my @aplog_ts = 0;
my $cur_aplog_ts = 0;
my $prev_aplog_ts = 0;
my $is_aplog = 0;

# argv[0]
if ($#ARGV < 0)
{
    print "usage: cmd input_file_name\n";
    exit;
}
my $input_file_name = $ARGV[0];
print "$input_file_name\n";

while ($pattern = <DATA>) {
    next if ($pattern =~ /^#/); #skip comments
    next if ($pattern =~ /^\s+/); #skip blank line 
    chomp $pattern;
    $regexp .= $pattern."|";
    #print $pattern,"\n";
}
$regexp .= "null_statements";

open INPUT_FILE_HANDL,$input_file_name or die "file not found\n";
while ($input_line = <INPUT_FILE_HANDL>) {
    if ($input_line =~ /Stdout/)
    {
        @aplog_ts = split /,/,$input_line;
#        printf "%d \n",$aplog_ts[1];

        $cur_aplog_ts = $aplog_ts[2];
        $is_aplog = 1;
    }
    
    if (($is_aplog == 0) && ($input_line =~ /\[\d+:\d+\]/))
    {
        @aplog_ts = split /\[|:|\]/,$input_line;
#        printf("......%d %d\n",$aplog_ts[1],$aplog_ts[2]);
        $cur_aplog_ts = $aplog_ts[1]+$aplog_ts[2]/1000.0;
    }
   
    if ($input_line =~ qr/$regexp/)
    {
        if ($prev_aplog_ts == 0 ) 
        {
            $prev_aplog_ts = $cur_aplog_ts;
        }
        # replace multiple space to one space
        $input_line =~ s/\s+/ /g;
        my $diff_aplog_ts = $cur_aplog_ts - $prev_aplog_ts;
#        printf "======%.3f %.3f======\n",$cur_aplog_ts , $prev_aplog_ts;
        if ($diff_aplog_ts > 0.030) 
        {
            printf "%.3f: keypoint %s\n",$diff_aplog_ts,$input_line;
        }else
        {
            printf "%.3f: %s\n",$diff_aplog_ts,$input_line;
        }
        $prev_aplog_ts = $cur_aplog_ts;
    }
}

print "========================================\n";
print "Finished\n";
print "========================================\n";

__DATA__
########################################
# This is a data file
frame through content object to MEDL
Enter.*::start\(
::execute
allocateDevice
MM_MEDL_START
.*
########################################

########################################
