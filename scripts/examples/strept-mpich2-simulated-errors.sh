mpirun -tag-output -np $(cat PROCESSES) ~/Ray/trunk/code/Ray \
-p /home/boiseb01/nuccore/strept_200_1.fastq \
   /home/boiseb01/nuccore/strept_200_2.fastq \
-o $0

~/Ray/trunk/scripts/print-latex.sh ~/nuccore/Streptococcus-pneumoniae-R6.fasta $0.fasta $0.Ray
