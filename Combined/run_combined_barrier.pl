#!/usr/bin/perl
use strict;
use warnings;
use vars;

use Data::Dumper;

chomp(my $pwd = `pwd`);
chomp(my $user = `whoami`);

my $tag = "combined_barrier";
my $count =0;

foreach my $node (2..8){
    foreach my $threads (2..12){
        open(my $wh,">","${tag}__mpinodes_${node}__ompthreads_${threads}.pbs") || die("\nCannot open a file to write. $!\n\n");

        print $wh '#PBS -N cs6210-proj2__'."${tag}__mpinodes_${node}__ompthreads_${threads}\.pbs\n";
        print $wh "\#PBS -l nodes\=${node}\:ppn\=${threads}\n";
        print $wh '#PBS -l pmem=1gb'."\n";
        print $wh '#PBS -l walltime=02:10:00'."\n";
        print $wh '#PBS -q pace-ice'."\n";
        print $wh '#PBS -j oe'."\n";
        print $wh "\#PBS \-o ${tag}__mpinodes_${node}__ompthreads_${threads}\.out\n";

        print $wh ("\n\n");
        print $wh 'echo "Started on `/bin/hostname`"'."\n";
        print $wh 'echo "Nodes chosen are:"'."\n";
        print $wh 'cat $PBS_NODEFILE'."\n";

        print $wh ("\n\n");
        print $wh 'module load gcc/8.3.0'."\n";
        print $wh 'module load mvapich2/2.3.2'."\n";
        print $wh "export OMP_NUM_THREADS\=${threads}\n";

        print $wh ("\n\n");
        print $wh "cd $pwd\n";
        print $wh "mpic\+\+ \-g \-Wall \-fopenmp \-lgomp ${tag}\.cpp \-o ${tag}__mpinodes_${node}__ompthreads_${threads}\n";
        print $wh "mpirun \-np $node \.\/${tag}__mpinodes_${node}__ompthreads_${threads} ${threads}\n";


        close($wh);
        `chmod 755 ${tag}__mpinodes_${node}__ompthreads_${threads}.pbs`;
        $count++;
    }
}

## Create the run.sh
    open(my $wh,">","run.cmd") || die("\nCannot open a file to write. $!\n\n");
    foreach my $node (2..8){
        foreach my $threads (2..12){
            print $wh "qsub ${tag}__mpinodes_${node}__ompthreads_${threads}.pbs\n";
        }
    }
    print $wh "qstat \| egrep \" R \| $user \"\n";
    close($wh);
    `chmod 755 run.cmd`;


## Launch the command.
##   `./run.cmd`;

#### keep polling until you get a keystroke.
##
##my $poll = 1;
##my %info;
##
##
###while($poll){
###
###    my @completed = `grep \"End PBS Epilogue\" \*\.out`;
##
##    if(scalar(@completed)>=$count){
## 
##        # Stop the polling.
##        $poll = 0;
##    
##        my @files = glob "*.out";
##        foreach my $file (@files){
##            chomp($file);
##            
##            my $total=0;
##            my $nodes;
##            my $threads;
##            
##            if($file=~m/${tag}__mpinodes_(\d+)__ompthreads_(\d+)\.out/){
##                $nodes = $1;  $threads=$2;
##            }
##
##            my @data = `grep \"avg barrier-time over\" $file`;
##            
##            # parse and extract the required data.
##            foreach(@data){            
##                chomp($_);
##                my @temp = split("\:",$_);
##                $total+=$temp[-1];
##            }
##               # Find the average. 
##               $info{$nodes} = sprintf("%.4f",($total)/scalar(@data));
##        }
##   
##    }else{
##        system( "qstat \| egrep \" R \| $user \"");
##        sleep(5);
##        system("clear");
##    }
##};
##

my %info;

   my @files = glob "*.out";

    foreach my $file (@files){
        chomp($file);
        
        my $total=0;
        my $nodes;
        my $threads;
        
        if($file=~m/${tag}__mpinodes_(\d+)__ompthreads_(\d+)\.out/){
            $nodes = $1;  $threads=$2;
        }

        my @data = `grep \"avg barrier-time over\" $file`;
        
        # parse and extract the required data.
        foreach(@data){            
            chomp($_);
            my @temp = split("\:",$_);
            $total+=$temp[-1];
        }
           # Find the average. 
           $info{$nodes}{$threads} = sprintf("%.4f",($total)/scalar(@data));
    }

    #print Dumper \%info;


### Dump the final report csv.
open($wh,">","${tag}_final-report.csv") || die ("\n\nCannot open a file to write\n\n");
   print $wh "$tag\n";
   print $wh "Nodes,threads,avg-barrier-time\(us\)\n";
   foreach my $node (sort {$a<=>$b} (keys %info)){
        foreach my $threads (sort {$a<=>$b} (keys %{$info{$node}})){
            print $wh "$node\,$threads\,$info{$node}{$threads}\n";
        }

        print $wh "\n";
    }

close($wh);

exit; 
    








