#!/bin/bash

#SBATCH -A SNIC2017-7-18
#SBATCH -o tests-%j.out
#SBATCH -p devel
#SBATCH -t 00:15:00
#SBATCH -N 1
#SBATCH -n 20
#SBATCH -J N01-test_FMM3D

#assert (ipn * nt  == 20 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 20  == -n ) 
#assert (  P * nt  == -n ) 

k=1;DLB=0
P=1;p=1;q=1;
ipn=$P;

nt=10;
to=2000

B=5;b=5;
iter=2

#assert ( B >= p ) 
#assert ( B >= q )



#====================================

#module load gcc openmpi
module load intel intelmpi
JobID=${SLURM_JOB_ID}
app=./bin/fmm3d_debug
data_path="./data/"
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --iter-no $iter --timeout $to --data-path ${data_path}" 
echo "Params: $params"
tempdir=./temp

mkdir -p $tempdir
outfile=$tempdir/tests_${JobID}.out
echo "========================================================================================="
set -x 
rm $outfile
if [ "z${CXX}z" == "zg++z" ] ; then 
    mpirun -n $P --map-by ppr:$ipn:node -output-filename $outfile   $app ${params}
else
    srun  -n $P --ntasks-per-node=$ipn -c $nt -m cyclic:cyclic:* -l --output $outfile $app ${params}
    for i in $(seq 0 $[$P-1])
    do
	if [ $i < 10 ]; then
	    i="0$i"
	fi
	grep "${i}:" $outfile >$tempdir/tests_${JobID}_p${i}.out
	str="s/$i://g"
	sed -i -e $str $tempdir/tests_${JobID}_p${i}.out
    done 
    echo "${JobID}:" >> timing.txt
    grep "First task submitted" $tempdir/tests_${JobID}_p0.out >> timing.txt
    grep "Program finished" $tempdir/tests_${JobID}_p0.out >> timing.txt
    grep "P:" $tempdir/tests_${JobID}_p0.out >> timing.txt
    grep " timeout" ${outfile}* 
fi
rm *file*.txt
rm core.*
rm pending* comm*