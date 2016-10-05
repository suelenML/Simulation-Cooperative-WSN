#!/bin/bash

./makemake
make depend
make clean 
make -j 5
