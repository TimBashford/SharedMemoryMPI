Shared Memory MPI
==============================

A boost::mpi implementation for ensuring a specified physical machine is acting as the control node.  Utilising rank 0 is naive because MPI itself assigns ranks to hostnames, but how this occurs is something of a black art.  This implementation allows the programmer to specify a hostname and compare MPI_COMM_RANK against hnID to ensure the head node runs serial processes.

Designed for dual-mode parallelism (MPI to the enclosure, internal threading); deliberately halts single-mode parallelism implementations, but may easily be modified to alter this behaviour if desired; simply removing the check will likely yield success, with rank 0 ceding control to a process on the appropriate enclosure based on rank order.

By Tim Bashford - 16/04/11.  Use as you see fit.