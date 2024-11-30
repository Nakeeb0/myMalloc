#!/usr/bin/perl
#!/usr/local/bin/perl

$out_filename = "coalescing3.rep";
$blk_size = 2047;
$num_iters = 2400;

# Open output file
open OUTFILE, ">$out_filename" or die "Cannot create $out_filename\n";

# Calculate misc parameters
$blk_size2 = 3*$blk_size;
$suggested_heap_size = 3*$blk_size*$num_iters + 100;
$num_blocks = 3*$num_iters + 1;
$num_ops = 8*$num_iters;

print OUTFILE "$suggested_heap_size\n";
print OUTFILE "$num_blocks\n";
print OUTFILE "$num_ops\n";
print OUTFILE "1\n";

for ($i = 0;  $i < $num_iters; $i += 1) {
    $blk1 = 3*$i;
    $blk2 = $blk1 + 1;
    $blk3 = $blk1 + 2;
    $blk4 = $blk1 + 3;

    print OUTFILE "a $blk1 $blk_size\n";
    print OUTFILE "a $blk2 $blk_size\n";
    print OUTFILE "a $blk3 $blk_size\n";
    print OUTFILE "f $blk1\n";
    print OUTFILE "f $blk3\n";
    print OUTFILE "f $blk2\n";
    print OUTFILE "a $blk4 $blk_size2\n";
    print OUTFILE "f $blk4\n";
}

close OUTFILE;

# a 1 64
# a 2 64
# a 3 64
# f 1 
# f 3 
# f 2 
# a 4 192
# f 4 
