#!/bin/bash

clear

cd "${0%/*}"

echo "Building the project..."
echo

./bin/build.sh

if [ $? -eq 0 ]; then
    echo "Project build successfully."
    echo
else 
    echo "Project build failed!"
    kill $$ SIGTERM
fi

echo "-------------------------------------------------"
echo

echo "Running the project..."
echo 
echo 

./bin/run.sh