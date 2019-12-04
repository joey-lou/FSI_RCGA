#!/bin/bash
gcc PV_cal.c -o PV_cal
gcc initialize.c -o initialize
gcc RCGA_eval.c -o RCGA_eval -lm
gcc flowvtk.c -o flowvtk

rm -r vtk/*

if [ ! -d "logged" ]; then
mkdir logged
fi
if [ ! -d "inputs/gen1" ]; then
mkdir inputs/gen1
fi
if [ ! -d "outputs/gen1" ]; then
mkdir outputs/gen1
fi
# initialize
POPULATION=30
MAXITER=100
./initialize $POPULATION
# for loops
for (( Gen=1; Gen<=MAXITER; Gen++ ))
	do		
		FLAG[0]=0 # 0 index flag reserved for while loop
		if [ ! -d "vtk/gen$Gen" ]; then
			mkdir vtk/gen$Gen
		fi
		for (( Ind=1; Ind<=POPULATION; Ind++))
		do
			# evaluate fsi for given generation and individual number
			cd Program_FIN$Ind
			
			./calc_fsi $Gen $Ind >/dev/null 2>&1 &
			ID[$Ind]=$!
			echo "Gen$Gen.Ind$Ind ID: ${ID[$Ind]}"
			FLAG[$Ind]=0
			cd ..
		done
		jobs
		while [ ${FLAG[0]} != 1 ]; do
			FLAG[0]=1
			for (( Ind=1; Ind<=POPULATION; Ind++))
			do
				if kill -0 ${ID[$Ind]}; then
					FLAG[0]=0
					echo "Gen$Gen.Ind$Ind still running"
				else
					if [ ${FLAG[$Ind]} != 1 ]; then
					# evaluate energy and velocity for given individual
						./PV_cal $Gen $Ind
						sleep 0.5
					# make vtk movie
						if [ `expr $Gen % 20` == 0 -o $Gen == 1  ]; then	
							if [ ! -d "vtk/gen$Gen/individual$Ind" ]; then
								mkdir vtk/gen$Gen/individual$Ind
							fi
							./flowvtk $Gen $Ind
						fi
						FLAG[$Ind]=1
					fi
				fi
				sleep 0.05
			done
			echo ""
			sleep 1
			if [ ${FLAG[0]} == 1 ]; then
				echo "Gen$Gen all process finished"
			fi
		done

	# now the current generation is calculated
		newGen=`expr $Gen + 1`
		if [ ! -d "inputs/gen$newGen" -a $Gen != $MAXITER ]; then
			mkdir inputs/gen$newGen
		fi
		if [ ! -d "outputs/gen$newGen" -a $Gen != $MAXITER ]; then
			mkdir outputs/gen$newGen
		fi
		./RCGA_eval $Gen $MAXITER $POPULATION 1 0
	done
