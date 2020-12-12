#!/bin/bash

test_num=0
REPORT_FILE=test_report.txt

echo "Remember to do these two commands before running this script:
export COLUMNS
export LINES
"
echo "Test run starting at $(date)" >>${REPORT_FILE}

#==============================================================================
#	Operations available in the Vacuum simulation.
#==============================================================================
function place_dust () {
	printf "d${1},${2},${3} " 
}

function place_slime () {
	printf "s${1},${2},${3} " 
}

function place_trash () {
	printf "t${1},${2},${3} " 
}

function move_vacuum () {
	printf "v${1},${2} " 
}

function set_timeout () {
	printf "o${1} "
}

function set_delay () {
	printf "m${1} "
}

function pause_resume () {
	printf "p "
}

function reset () {
	printf "r${1} "
}

function left () {
	printf "j "
}

function right () {
	printf "l "
}

function up () {
	printf "i "
}

function down () {
	printf "k "
}

function quit () {
	printf "q "
}

function help () {
	printf "? " 
}

function battery_cheat () {
	printf "y${1} " 
}

function place_dust () {
	printf "d${1},${2},${3} " 
}

function loop () {
	if (( $# >= 1 )) && (( $1 > 0 )) 
	then
		for (( i = 0; i < $1; i++ ))
		do
			printf " "
		done
	else
		printf " "
	fi
}

function enter () {
	printf "${1} "
}


#========================================================================
#	Run a single test.
#========================================================================	

function run_test () {
	(( test_num ++ ))
	echo "Test    : ${test_num}"
	echo "Category: ${category}"
	echo "Details : ${details}"
	echo "Expect  : ${expect}"
	
	read -p "Press r to run the test or s to skip..." run_or_skip
	
	if [ "${run_or_skip}" == "r" ]
	then
		echo ${cmd} | ./hello
		read -p "Did the test pass or fail (p/f)? " result
		echo "Test ${test_num}: ${result}" >>${REPORT_FILE}
	fi
}

#==============================================================================
category="Setup Vacuum"
details="Vacuum in center of small terminal window."
expect="Vacuum is displayed in centre; 1 dust, 1 slime and 1 trash is visible at random location."

cmd=$( echo "1,1,1 o5 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Setup Dust"
details="Vacuum, 2 dust, 1 slime and 1 trash"
expect="Vacuum is displayed in centre; 2 dust, 1 slime and 1 trash is visible at random location."

cmd=$( echo "2,1,1 o5 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Setup Slime"
details="Vacuum, 1 dust, 2 slime and 1 trash"
expect="Vacuum is displayed in centre; 1 dust, 2 slime and 1 trash is visible at random location."

cmd=$( echo "1,2,1 o5 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Setup Trash"
details="Vacuum, 1 dust, 1 slime and 2 trash"
expect="Vacuum is displayed in centre; 1 dust, 1 slime and 2 trash is visible at random location."

cmd=$( echo "1,1,2 o5 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Display Maximum Number of Dust"
details="Vacuum, 1000 dust, 1 slime and 1 trash"
expect="Vacuum is displayed in centre; Maximum number of dusts, 1 slime and 1 trash is visible at random location."

cmd=$( echo "1000,1,1 o5 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Display Maximum Number of Slime"
details="Vacuum, 1 dust, 10 slime and 1 trash"
expect="Vacuum is displayed in centre; 1 dust, Maximum number of slimes and 1 trash is visible at random location."

cmd=$( echo "1,10,1 o10 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Display Maximum Numbner of Trash"
details="Vacuum, 1 dust, 1 slime and 5 trash"
expect="Vacuum is displayed in centre; 1 dust, 1 slimes and Maximum number of trash is visible at random location."

cmd=$( echo "1,1,5 o10 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Time Out Test"
details="Time-out after 5 seconds"
expect="Close window after 5 seconds"

cmd=$( echo "1,1,1 o5 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Do Vacuum Cheat"
details="Vacuum placed at user input location."
expect="Vacuum is displayed in centre; 1 dust, 1 slimes and Maximum number of trash is visible at random location."

cmd=$( echo "0,0,0 o5 v1,5,0")

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Place Dust at particular Position"
details="Dust is place at user input locations."
expect="Vacuum is displayed in centre; 1 dust at position (2,6), 1 slimes and 1 trash is visible at random location."

cmd=$( echo "1,1,1 o5 d0,2,6")

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Place Slime at particular Position"
details="Slime is place at user input locations."
expect="Vacuum is displayed in centre; 1 dust at random position, 1 slimes at (2,6) and 1 trash is visible at random location."

cmd=$( echo "1,1,1 o5 s0,2,6")

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Place Trash at particular Position"
details="Trash is place at user input locations."
expect="Vacuum is displayed in centre; 1 dust and, 1 slimes at random position and 1 trash at (2,6) location."

cmd=$( echo "1,1,1 o5 t0,2,6")

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'j'"
details="Movement of vacuum"
expect="Vacuum is displayed in centre; rubbish at random position; 1 step is moved in left direction when 'j' is pressed."

cmd=$( echo "1,1,1 o5 j " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'l'"
details="Movement of vacuum"
expect="Vacuum is displayed in centre; rubbish at random position; 1 step is moved in right direction when 'l' is pressed."

cmd=$( echo "1,1,1 o5 l " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'i'"
details="Movement of vacuum"
expect="Vacuum is displayed in centre; rubbish at random position; 1 step is moved in up direction when 'i' is pressed. "

cmd=$( echo "1,1,1 o5 i " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'k'"
details="Movement of vacuum"
expect="Vacuum is displayed in centre; rubbish at random position; 1 step is moved in left direction when 'k' is pressed. "

cmd=$( echo "1,1,1 o5 k " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'r'"
details="Reset Simulation"
expect="Vacuum is displayed in centre; rubbish at random position. "

cmd=$( echo "1,1,1 o5 r " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'p'"
details="Movement of vacuum"
expect="Vacuum is displayed in centre; rubbish at random position; vacuum moves in random direction after pressing 'p'. "

cmd=$( echo "1000,0,0 o5"; echo " p" )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'y'"
details="Battery Cheat"
expect="Vacuum is displayed in centre; rubbish at random position; battery is set to user input value after pressing 'y'. "

cmd=$( echo "0,0,0 o5 y45 " )

set_timeout 10

run_test
#==============================================================================

#==============================================================================
category="Press of 'q'"
details="Game quit"
expect="Simulation get closed after pressing of 'q' "

cmd=$( echo "0,0,0 o5 q " )

set_timeout 100

run_test
#==============================================================================

