Compile with the following statement:
    
    gcc -o smallsh main.c built_in.c

Run with the following command:

    ./smallsh

TO GRADER:

    I was basically doing great on everything in the assignment up until
managing the `&` symbols for the background processes. If you go through
the files you'll notice that functions for all of the assignment's scope
exist in the code, being careful to remember warnings Ben told us in class.
However, the test script was having trouble with some of the variables
(printing them specifically), and there is some erroneous and glitchy output
towards the end of the file. I spent a looooooong night trying to debug this
(and woke up early in the morning too), but to no avail. If any partial credit 
for the pID getting of processes is possible, please look at the functions
and their comments (some of which are from the assignment, see the header files),
it would be greatly appreciated. Just ran out of time at the end, with my
Capstone and UX projects eating up a lot of my time the past couple of weeks.

Thanks,
Stef



Running the test script with the commands from the assignment should work.

See header files for documentation. Documentation was either
handwritten or take from the assignment page (a lot of times
elaborating on what a function did felt unnessary but inline
comments can be found throughout).

Block comments in header files in this format are from the assignment:
/*
This is a comment!
*/

Block comments in header files in this format are from me:
/*
* This is a comment!
*/
