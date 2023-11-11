#!/bin/bash

#Check if an argument is provided
if [ $# -eq 0 ]; then
  echo "No output directory is set! Usage: $0 [output directory]"
  exit 1
fi

# The output directory is the first argument
outputDir="$1/shaders"
# Clear the output directory
if [ -d "$outputDir" ]; then
    rm -f "$outputDir"/*
else
    mkdir -p "$outputDir"
fi

#Define the directory where the shaders live in
shaderDir="$(dirname "$0")/modules/jarep-renderer/shaders"

echo "Searching for glslc executable..."

keywords=("vulkan", "vulkan-sdk", "sdk")

# Search for all instances of glslc in the filesystem
glslcPaths=$(find / -type f -executable -name glslc 2>/dev/null)

# Convert the paths into an array
readarray -t glslcArray <<< "$glslcPaths"

# Filter paths based on keywords
filteredPaths=()
for path in "${glslcArray[@]}"; do
    for keyword in "${keywords[@]}"; do
        if [[ "$path" == *"$keyword"* ]]; then
            filteredPaths+=("$path")
            break # Breaks the inner loop if a keyword is found
        fi
    done
done

# Check the filtered results
if [ ${#filteredPaths[@]} -eq 0 ]; then
    echo "No glslc executable found."
    exit 1
elif [ ${#filteredPaths[@]} -eq 1 ]; then
    # Only one matching glslc found
    glslcPath=${filteredPaths[0]}
else
    # Multiple matching glslc executables found
    echo "Multiple glslc executables found:"
    for path in "${filteredPaths[@]}"; do
        echo " - $path"
    done
    exit 1
    # Additional logic to select one executable can be added here
fi

echo "glslc executable found."
echo "Searching for spirv-cross executable..."
# Search for all instances of spirv-cross in the filesystem
spirvCrossPaths=$(find / -type f -executable -name spirv-cross 2>/dev/null)

# Convert the paths into an array
readarray -t spirvCrossArray <<< "$spirvCrossPaths"

# Filter paths based on keywords
filteredSpirvCrossPaths=()
for path in "${spirvCrossArray[@]}"; do
    for keyword in "${keywords[@]}"; do
        if [[ "$path" == *"$keyword"* ]]; then
            filteredSpirvCrossPaths+=("$path")
            break # Breaks the inner loop if a keyword is found
        fi
    done
done

# Check the filtered results
if [ ${#filteredSpirvCrossPaths[@]} -eq 0 ]; then
    echo "No spirv-cross executable found."
    canCompileToMetal=false
elif [ ${#filteredSpirvCrossPaths[@]} -eq 1 ]; then
    # Only one matching glslc found
    spirvCrossPath=${filteredSpirvCrossPaths[0]}
    canCompileToMetal=true
else
    # Multiple matching glslc executables found
    echo "Multiple spirv-cross executables found:"
    for path in "${spirvCrossArray[@]}"; do
        echo " - $path"
    done
    canCompileToMetal=false
    # Additional logic to select one executable can be added here
fi

echo "Spirv-cross found ($spirvCrossPath)"

compile_shader(){
    inputFile=$1
    filename=$(basename -- "$inputFile")
    extension="${filename##*.}"
    filename="${filename%.*}"

    # Compile to SPIR-V format (.spv)
    spvOutputFile="$outputDir/${filename}_${extension}.spv"
    $glslcPath "$inputFile" -o "$spvOutputFile"


   # Check if Metal shader compilation is possible
   if [ "$canCompileToMetal" = true ]; then
       # Convert the SPIR-V file to Metal shading language (.metal)
       metalOutputFile="$outputDir/${filename}_${extension}.metal"
       $spirvCrossPath "$spvOutputFile" --msl --output "$metalOutputFile"
   fi
}

export -f compile_shader

# Find all .vert and .frag files and store in an array
shaders=($(find "$shaderDir" -type f \( -name "*.vert" -o -name "*.frag" \)))
numShaders=${#shaders[@]}

# Check if shaders are found
if [ $numShaders -eq 0 ]; then
    echo "No shaders found in the directory."
    exit 1
fi

# Display the number of shaders found
echo "$numShaders shaders found. Starting compilation..."

# Compile shaders and display a progress bar
for (( i=0; i<$numShaders; i++ )); do
    compile_shader "${shaders[i]}"
    # Update progress
    echo -ne "\rCompiling shaders: $((i + 1))/$numShaders"
done

# Clear the line and display a completion message
echo -e "\rAll shaders compiled successfully."


