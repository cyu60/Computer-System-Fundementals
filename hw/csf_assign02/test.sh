#! /usr/bin/env bash

for (( counter=5; counter>0; counter-- ))
do
ruby gentest.rb 
done
printf "\n"