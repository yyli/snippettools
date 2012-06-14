# Timer C++ Class

Wrapper for `clock_gettime`

## USAGE
Initialized Timer object:
`Timer t1;`

Starts the Timer:
`t1.start();`

Ends the Timer:
`t1.end();`

Returns the elapsed time as a double:
`double time = t1.elapsed();`

Returns the time until now as double;
`double time = t1.timePassed();`

