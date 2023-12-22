#!/bin/bash

# Check if the correct number of parameters is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <destination_directory>"
    exit 1
fi

# Assign parameters to variables
destination_directory="$1"

# Check if destination directory exists, if so, abort
if [ -d "$destination_directory" ]; then
    echo "Destination directory '$destination_directory' already exists. Aborting."
    exit 1
fi

mkdir -p "$destination_directory"
echo "Destination directory '$destination_directory' created."

# Copy files to destination
cp ./main.cpp "$destination_directory"
touch "$destination_directory"/data.txt "$destination_directory"/test_data.txt
