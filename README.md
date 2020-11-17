# RBTree
A c library implementing a generic Red Black Tree (using void pointers).
The tree is created with a pointers to two functions, one for comparing two data elements,
and one for freeing the memory allocated for the data. The tree support inserstion, deletion
and searching, all in logarithmic complexity. The tree also supports a "for each" function
through which you can perform an action/calculation on all of the elements.
The library was implemented for a school assignment.

# Usage
Include the header file in your code and compile RBTree.c with your
code, or compile it as a library.
