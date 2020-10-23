#!/usr/bin/perl
use strict;
use warnings;
use vars;

use Data::Dumper;

chomp(my $pwd = `pwd`);
chomp(my $user = `whoami`);

my $tag = "omp_MCS_barrier";
my $count =0;

foreach my $threads (2..16){
    open(my $wh,">","${tag}__ompthreads_${threads}.pbs") || die("\nCannot open a file to write. $!\n\n");

    print $wh '#PBS -N cs6210-proj2__'."${tag}__ompthreads_${threads}\.pbs"."\n";
    print $wh "\#PBS -l nodes\=1\:ppn\=${threads}\n";
    print $wh '#PBS -l pmem=1gb'."\n";
    print $wh '#PBS -l walltime=00:02:00'."\n";
    print $wh '#PBS -q pace-ice'."\n";
    print $wh '#PBS -j oe'."\n";
    print $wh "\#PBS \-o ${tag}__ompthreads_${threads}\.out\n";

    print $wh ("\n\n");
    print $wh 'echo "Started on `/bin/hostname`"'."\n";
    print $wh 'echo "Nodes chosen are:"'."\n";
    print $wh 'cat $PBS_NODEFILE'."\n";

    print $wh ("\n\n");
    print $wh 'module load gcc/8.3.0'."\n";
    print $wh "export OMP_NUM_THREADS\=${threads}\n";

    print $wh ("\n\n");
    print $wh "cd $pwd\n";
    print $wh "g\+\+ \-g \-Wall \-fopenmp ${tag}\.cpp \-o ${tag}__ompthreads_${threads}\n";
    print $wh "\.\/${tag}__ompthreads_${threads} ${threads} \n";


    close($wh);
    `chmod 755 ${tag}__ompthreads_${threads}.pbs`;
    $count++;
}

## Create the run.sh
    open(my $wh,">","run.cmd") || die("\nCannot open a file to write. $!\n\n");
    foreach my $threads (2..16){
        print $wh "qsub ${tag}__ompthreads_${threads}.pbs\n";
    }
    print $wh "qstat \| egrep \" R \| $user \"\n";
    close($wh);
    `chmod 755 run.cmd`;


## Launch the command.
    `./run.cmd`;

## keep polling until you get a keystroke.

my $poll = 1;
my %info;


while($poll){

    my @completed = `grep \"End PBS Epilogue\" \*\.out`;

    if(scalar(@completed)>=$count){
    
        # Stop the polling.
        $poll = 0;
    
        my @files = glob "*.out";
        foreach my $file (@files){
            chomp($file);
            
            my $total=0;
            my $threads;
            ($threads = $1) if($file=~m/${tag}__ompthreads_(\d+)\.out/);

            my @data = `grep \"avg barrier-time over\" $file`;
            
            # parse and extract the required data.
            foreach(@data){            
                chomp($_);
                my @temp = split("\:",$_);
                $total+=$temp[-1];
            }
               # Find the average. 
               $info{$threads} = sprintf("%.4f",($total)/scalar(@data));
        }
   
    }else{
        system( "qstat \| egrep \" R \| $user \"");
        sleep(5);
        system("clear");
    }
};


# Dump the final report csv.
open($wh,">","${tag}_final-report.csv") || die ("\n\nCannot open a file to write\n\n");
   print $wh "$tag\n";
   print $wh "threads,avg-barrier-time\(us\)\n";
   foreach (sort {$a<=>$b} (keys %info)){
      print $wh "$_\,$info{$_}\n";
    }

close($wh);

exit; 
    








