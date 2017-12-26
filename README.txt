Ethan Schutzman && Steven Mcateer
    ehschutzman && smcateer

This is the v0 - v2 for Midday Commander
The program runs with the document specified menu, it is compiled using the make command
which will create an executable files called m0 m1 and m2.

USAGE:: To run the program run the make command

./mc0
./mc1
./mc2

You are able to pipe in an input file to run test commands without input

Follow the menu instructions to use the program:

to quit phase1 press ctrl C (^C) 

to quit phases 2 or 3 simply enter 'e'


You are able to add up to 6 Custom commands.

You are able to execute them in the foreground infinitely, but only 100 times total in the background

When adding a command for use in the background make the last charachter is an '&': 

Example:  sleep 10& 


To keep track of background jobs we used an array to store the pids of all jobs executed in the backgroud.  We kept track of active jobs.

To test the programs we used user input as well as piping in text files with user slections.  The output files look a little strange because execution takes less time than writing so for some cases it executes before it writes the previous data.

