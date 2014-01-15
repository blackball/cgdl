cgdl  

![alt clusters](https://github.com/blackball/cgdl/raw/master/data/clusters.bmp)  
![alt result](https://github.com/blackball/cgdl/raw/master/data/result.bmp)  

This is an implementation of the agglomerative clustering algorithm   
presented in [0]. There's an example presented in *example.c*. It   
simply grouped the pixels in the first picture using *GDL*, the second   
picture is the output. 

The codes were tested both on Win7 using VS2010 and Linux (Ubuntu 12.04), 
and it should work in many other systems too. You need install the 
OpenCV (1.0+) library to compile *example.c*.  

*On Windows*: Just add all source files into a project, setup the OpenCV directories, compile.  
*On Linux*: Type *make* to compile, then run the demo with *./cgdl-demo* .
  
The author also open sourced his Matlab implementation in :  
    https://github.com/waynezhanghk/gactoolbox  
   
[0]: Graph Degree Linkage: Agglomerative Clustering on a Directed Graph, Wei Zhang. ECCV 2012.  
