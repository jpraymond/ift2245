-The file address.txt has 1000 pseudo-random values in the range [0 , 65535] that are clustered around 16 uniformly distributed values. This is to simulate spacial coherence in the access to memory, so that the TLB-hit is expected to be high, as we are constantly accessing the same pages that can be directly resolved by the TLB.

-On the other side the file addressRANDOM.txt has 70,000 completely random values in the range [0 , 65535], that does not represent regular memory acces, but can be helpful to verify that at some point all the physical memory is full.

-Each address file comes with a correctOutput.txt corresponding file.