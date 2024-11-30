#!/usr/bin/perl
#!/usr/local/bin/perl
#
use Math::Random::NormalDistribution;

$out_filename = $argv[0];
$out_filename = "random3.rep" unless $out_filename;
$num_blocks = $argv[1];
# $num_blocks = 1200 unless $num_blocks;
$num_blocks = 2400 unless $num_blocks;
$max_blk_size = $argv[2];
$max_blk_size = 32768 unless $max_blk_size;

print "Output file: $out_filename\n";
print "Number of blocks: $num_blocks\n";
print "Max block size: $max_blk_size\n";

$first_random = 600;
$gaussian_first = 1200;
$gaussian_second = 1800;
$gaussian_third = 800;
$total_blocks_new = $first_random + $gaussian_first + $gaussian_second + $gaussian_third;

# Create trace
# Make a series of malloc()s
for ($i = 0;  $i < $first_random; $i += 1) {
    $size = int(rand $max_blk_size);
    $op = {};
    $op->{type} = "a";
    $op->{seq} = $i;
    $op->{size} = $size;
    $total_block_size += $size;
    push @trace, $op;
}
# Insert free()s in proper places
for ($i = 0;  $i < $first_random; $i += 1) {
    for ($minval = $i; $minval < $first_random + $i; $minval += 1) {
        if (($trace[$minval]->{type} eq "a") && ($trace[$minval]->{seq} == $i)) {
            last;
        }
    }
    $pos = int(rand($first_random + $i - $minval - 1) + $minval + 1);
    $op = {};
    $op->{type} = "f";
    $op->{seq} = $i;
    splice @trace, $pos, 0, $op;
}

$generator = rand_nd_generator(64,10);
# Create trace
# Make a series of malloc()s
for ($i = 0;  $i < $gaussian_first; $i += 1) {
    $size = int($generator->());
    $op = {};
    $op->{type} = "a";
    $op->{seq} = $i;
    $op->{size} = $size;
    $total_block_size += $size;
    push @trace2, $op;
}
# Insert free()s in proper places
for ($i = 0;  $i < $gaussian_first; $i += 1) {
    for ($minval = $i; $minval < $gaussian_first + $i; $minval += 1) {
        if (($trace2[$minval]->{type} eq "a") && ($trace2[$minval]->{seq} == $i)) {
            last;
        }
    }
    $pos = int(rand($gaussian_first + $i - $minval - 1) + $minval + 1);
    $op = {};
    $op->{type} = "f";
    $op->{seq} = $i;
    splice @trace2, $pos, 0, $op;
}

$generator = rand_nd_generator(2048,200);
# Create trace
# Make a series of malloc()s
for ($i = 0;  $i < $gaussian_second; $i += 1) {
    $size = int($generator->());
    $op = {};
    $op->{type} = "a";
    $op->{seq} = $i;
    $op->{size} = $size;
    $total_block_size += $size;
    push @trace3, $op;
}
# Insert free()s in proper places
for ($i = 0;  $i < $gaussian_second; $i += 1) {
    for ($minval = $i; $minval < $gaussian_second + $i; $minval += 1) {
        if (($trace3[$minval]->{type} eq "a") && ($trace3[$minval]->{seq} == $i)) {
            last;
        }
    }
    $pos = int(rand($gaussian_second + $i - $minval - 1) + $minval + 1);
    $op = {};
    $op->{type} = "f";
    $op->{seq} = $i;
    splice @trace3, $pos, 0, $op;
}

$generator = rand_nd_generator(512,80);
# Create trace
# Make a series of malloc()s
for ($i = 0;  $i < $gaussian_third; $i += 1) {
    $size = int($generator->());
    $op = {};
    $op->{type} = "a";
    $op->{seq} = $i;
    $op->{size} = $size;
    $total_block_size += $size;
    push @trace4, $op;
}
# Insert free()s in proper places
for ($i = 0;  $i < $gaussian_third; $i += 1) {
    for ($minval = $i; $minval < $gaussian_third + $i; $minval += 1) {
        if (($trace4[$minval]->{type} eq "a") && ($trace4[$minval]->{seq} == $i)) {
            last;
        }
    }
    $pos = int(rand($gaussian_third + $i - $minval - 1) + $minval + 1);
    $op = {};
    $op->{type} = "f";
    $op->{seq} = $i;
    splice @trace4, $pos, 0, $op;
}

# Open output file
open OUTFILE, ">$out_filename" or die "Cannot create $out_filename\n";

# Calculate misc parameters
$suggested_heap_size = $total_block_size + 100;
$num_ops = 2*$total_blocks_new;

print OUTFILE "$suggested_heap_size\n";
print OUTFILE "$gaussian_second\n";
print OUTFILE "$num_ops\n";
print OUTFILE "1\n";

for ($i = 0;  $i < 2*$first_random; $i += 1) {
    if ($trace[$i]->{type} eq "a") {
        print OUTFILE "$trace[$i]->{type} $trace[$i]->{seq} $trace[$i]->{size}\n";
    } else {
        print OUTFILE "$trace[$i]->{type} $trace[$i]->{seq}\n";
    }
}

for ($i = 0;  $i < 2*$gaussian_first; $i += 1) {
    if ($trace2[$i]->{type} eq "a") {
        print OUTFILE "$trace2[$i]->{type} $trace2[$i]->{seq} $trace2[$i]->{size}\n";
    } else {
        print OUTFILE "$trace2[$i]->{type} $trace2[$i]->{seq}\n";
    }
}

for ($i = 0;  $i < 2*$gaussian_second; $i += 1) {
    if ($trace3[$i]->{type} eq "a") {
        print OUTFILE "$trace3[$i]->{type} $trace3[$i]->{seq} $trace3[$i]->{size}\n";
    } else {
        print OUTFILE "$trace3[$i]->{type} $trace3[$i]->{seq}\n";
    }
}

for ($i = 0;  $i < 2*$gaussian_third; $i += 1) {
    if ($trace4[$i]->{type} eq "a") {
        print OUTFILE "$trace4[$i]->{type} $trace4[$i]->{seq} $trace4[$i]->{size}\n";
    } else {
        print OUTFILE "$trace4[$i]->{type} $trace4[$i]->{seq}\n";
    }
}


close OUTFILE;

