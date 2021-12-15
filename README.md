# EPFL CS-323 Lab

My solution to EPFL CS-323: Introduction to Operating Systems. Solutions are
located in the `done/` subdirectory of each lab.

## Instructions

A Dockerfile is provided for a functioning build environment. Run these commands
to attach to a terminal within the container.

```
docker build -t cs323 .
docker run -it --name=oslab -v ${shell pwd}:/src cs323 /bin/bash
```
