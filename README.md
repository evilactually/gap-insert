Given a balanced binary tree augmented with counts, it is possible to find a gap between existing nodes and create a new node there in logarithmic time. This algorithm can be used to generate unique keys, handles, ids for small key spaces or where binary tree data structure is already used. It is not as fast or memory efficient as using pseudo-random numbers, but it proves that it is possible to generate guaranteed unique keys in logarithmic time!

This repository is a reference implementation of the algorithm in C. Gap insertion is implemented in **rbst.c** as **rbst_gap_insert** function. The binary tree itself is balanced by inserting into random gaps(no user generated keys). This technique is sufficient for this particular use case, but nothing prevents from using same algorithm on a red-black tree or any other type, the only requirement is that node counts must be available in constant time(i.e tree must be count-augmented).

Files **uniquemap.c** and **.h**  define a data structure that allows associating unique integer handles with pointers. It's based on **rbst.c** tree and is presented here as an example of potential usage.


