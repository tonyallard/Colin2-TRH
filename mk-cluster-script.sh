#!/bin/bash

STATESDIR=/state/partition1/clustershare/phaslum/states
EXPDIR=/state/partition1/clustershare/phaslum/TRH
PLANNER=/state/partition1/clustershare/phaslum/bin/colin-TRH

states=`ls $STATESDIR/*.pddl | grep -e '[0-9]\\+\\.pddl'`
njobs=`ls $STATESDIR/*.pddl | grep -e '[0-9]\\+\\.pddl' | wc -l`

jobnum=0

# args: $1 = name; $2 = domain file; $3 = problem file
# log is written to $cwd/$name.log
make_job() {
    jobnum=$((jobnum + 1))
    echo "if [[ \$SGE_TASK_ID == $jobnum ]]; then"
    echo "  echo \"at \`date\` starting job $1\""
    echo "  ${PLANNER} $2 $3 > ${1}.log"
    echo "  echo \"at \`date\` finished job $1\""
    echo "fi"
}

## output preamble

echo "#! /bin/bash"
echo "### Set shell."
echo "#$ -S /bin/bash"
echo "### Don't send emails."
echo "#$ -m n"
echo "### Execute job from current working directory."
echo "#$ -cwd"
echo "### redirect stdout and stderr"
echo "#$ -e $EXPDIR/run.err"
echo "#$ -o $EXPDIR/run.log"
echo "### Set timeout."
echo "#$ -l h_cpu=1800"
echo "### Number of tasks"
echo "#$ -t 1-${njobs}"
echo ""
echo "cd $EXPDIR"
echo ""

for pfile in $states; do
    pname=`basename $pfile .pddl`
    dfile=${STATESDIR}/${pname}domain.pddl
    make_job $pname $dfile $pfile
done
