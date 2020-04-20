## Compile
`g++ -std=c++11 -o routing routing.cpp`

## Execute
The same format as mentioned in the assignment:
`./routing -N switchportcount -B buffersize -p packetgenprob -queue INQ|KOUQ|ISLIP -K knockout -out outputfile -T maxtimeslots`

## Output
Output file created in the same directory as the code.

## Additional Features
- runAll() function created to run experiments for N = 4, 5, ..., 100 in a single run.
- verbose flag to display step-by-step details on commandline.
- option to generate a different/same random number everytime.
To use additional features, see comments in the main() function in the code.