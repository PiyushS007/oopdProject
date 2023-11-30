#!/bin/bash

# Function to create files with specified size and count
create_files() {
    local target_dir="$1"
    local file_count="$2"
    local file_size="$3"

    mkdir -p "$target_dir"
    cd "$target_dir"

    time (
        for ((i = 1; i <= file_count; i++)); do
            dd if=/dev/zero of="file${i}.txt" bs="$file_size" count=1
        done
    )

    cd ..
}

# Create a directory to hold 30 files of 1GB each
create_files "dir1" 30 1G

# Create a directory to hold 1000 files of 10MB each
create_files "dir2" 1000 10M

# Create a directory structure with 100 files of 10MB each in subdirectories
create_files_recursive() {
    local target_dir="$1"
    local file_count="$2"
    local depth="$3"

    mkdir -p "$target_dir"
    cd "$target_dir"

    if [ "$depth" -eq 0 ]; then
        return
    fi

    # Create files in the current directory
    for ((i = 1; i <= file_count; i++)); do
        dd if=/dev/zero of="file${i}.txt" bs=10M count=1
    done

    # Create subdirectories and recursively create files within them
    for ((i = 1; i <= file_count; i++)); do
        mkdir -p "subdir${i}"
        cd "subdir${i}"
        create_files_recursive "$file_count" "$((depth - 1))"
        cd ..
    done
}

create_files_recursive "dir3" 50 2

# Count the total number of files (excluding directories)
total_files=$(find . -type f | wc -l)
echo "Total files created: $total_files"
