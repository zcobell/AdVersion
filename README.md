# AdVersion

##Introduction
AdVersion serves as a proxy for versioning ADCIRC model meshes using Git. The code seeks to create a mesh format other than the standard ADCIRC mesh format that:

1. Reduces the changes caused to the file by editing a mesh
2. Provides a efficient algorithm, and
3. Is easy to use

##Dependencies

| Package | Source |
|---------|--------|
|QADCModules | [https://github.com/zcobell/QADCModules.git]([https://github.com/zcobell/QADCModules.git)|
|METIS*|[http://glaros.dtc.umn.edu/gkhome/metis/metis/overview](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview)|
|libGit2|[https://libgit2.github.com/](https://libgit2.github.com/)|
|Qt|[http://qt.io](http://qt.io)|
*Note: Metis is automatically incorporated for Windows, and it is generally available via package managers on Unix

##Methodology

###Removing Node Numbers
In order to create a unique ID that is immutable, AdVersion uses cryptographic hashes constructed from the nodal positioning. This allows the code to generate a completely unique identifier for a node that does not change in time, and thus, will not change the mesh numbering

###File Storage
Files are stored using a series of directories. ADCIRC Mesh nodes and elements are partitioned using METIS into a user specified number of domains. These partitions are generalized into rectangles so that future nodal and elemental searches occur in O(n) time instead of O(n^2) time.

By partitioning the mesh into small subdomains, mesh edits generally create trivial changes to the files within the folder structure making the new format an excellent candidate for versioning using Git.

###Disclosure
The code is not production ready. Any use is at the user's own risk and I take no responsibility for meshes that become unrecoverable. If you do encounter an issue, please submit a bug report so the first release version can be improved.
