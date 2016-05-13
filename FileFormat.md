#AdVersion File Format
Adversion decomposes ADCIRC meshes across multiple partitions. The following file
describes the formats used to create the file format

##Creating Hash Values
One critical part of the process for creating unique identifiers for use with
the versioning software is creating cryptographic hashes. The format specifiers
for this process are critical to ensure that two programs can arrive at the same 
hash. The hash functions used within this code are embedded within the QADCModules
library, but this code dictates how it was implemented there.

###Nodes
Nodes are hashed by passing the X and Y (not Z) to the chosen hash algorithm. The
x and y values are written to a string using the following format:
```
%+18.12e
```
The hash is constructed by successively passing data to the hash algorithm. The
code used is below
```cpp
hash.addData(x_string.toUtf8(),hashSeed1.length());
hash.addData(y_string.toUtf8(),hashSeed2.length());
```

###Elements
Elements are hashed by passing the node hashes for each node into the hash algorithm
in its raw form. The code used is below:
```cpp
hash.addData(node1->positionHash);
hash.addData(node2->positionHash);
hash.addData(node3->positionHash);
```

###Boundaries
Boundaries are created so that any change along the boundary ensures that a new hash is created. 
The boundary hash is constructed using the following steps:

1.  Hash the boundary code integer
2.  For each node on the boundary, the data is hashed in the order that most closely mathes the standard Adcirc format. Add the data to the hash in this way:
  1. Node 1 hash
  2. Node 2 hash [if applicable]
  3. Weir parameters in the standard Adcirc ordering (Formatted using ``%+018.12e``)
