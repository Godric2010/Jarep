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

find_executable(){
  local executableName=$1
  local executablePath

  case "$(uname)" in
    "Linux")
      executablePaths=$(find ~/vulkan-sdk -type f -executable -name "$executableName" 2>/dev/null)
    ;;
    "Darwin")
      executablePaths=$(which "$executableName" 2>/dev/null)
    ;;
    "CYGWIN"*|"MINGW"*|"MSYS"*)
      executablePaths=$(where "$executableName" 2>/dev/null)
    ;;
    *)
      echo "Unsupported OS"
      return 1
    ;;
    esac

  executablesArray=()
  while IFS= read -r line; do
    executablesArray+=("$line")
  done <<< "$executablePaths"

  if [ ${#executablesArray[@]} -eq 0 ]; then
    echo "$executableName executable found."
    exit 1
  elif [ ${#executablesArray[@]} -eq 1 ]; then
    executablePath=${executablesArray[0]}
  else
    # Multiple matching glslc executables found
    echo "Multiple glslc executables found:"
    for path in "${executablesArray[@]}"; do
        echo " - $path"
    done
    executablePath=${executablesArray[0]}
  fi

  echo "$executablePath"
  return 0
}

echo "Searching for glslc executable..."

# Search for all instances of glslc in the filesystem
#glslcPaths=$(find / -type f -executable -name glslc 2>/dev/null)
glslcPath=$(find_executable "glslc")

echo "glslc executable found."
echo "Searching for spirv-cross executable..."
# Search for all instances of spirv-cross in the filesystem
spirvCrossPath=$(find_executable "spirv-cross")

echo "Spirv-cross found ($spirvCrossPath)"

compile_shader(){
    inputFile=$1
    filename=$(basename -- "$inputFile")
    extension="${filename##*.}"
    filename="${filename%.*}"

    # Compile to SPIR-V format (.spv)
    spvOutputFile="$outputDir/${filename}_${extension}.spv"
    $glslcPath "$inputFile" -o "$spvOutputFile"

    # Convert the SPIR-V file to Metal shading language (.metal)
    metalOutputFile="$outputDir/${filename}_${extension}.metal"
    $spirvCrossPath "$spvOutputFile" --msl --msl-decoration-binding --output "$metalOutputFile"
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


