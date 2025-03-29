import os
import subprocess

SHADER_DIR = os.path.join(os.path.dirname(__file__), "shaders")
EXTENSIONS = [".vert", ".frag"]

output_flag = os.environ.get("CMAKE_COMPILED_FLAG_PATH", "compiled.flag")
BUILD_OUTPUT_DIR = os.path.join(os.getcwd(), "Resources", "Shaders")
os.makedirs(BUILD_OUTPUT_DIR, exist_ok=True)

def compile_shader(shader_file):
    spv_name = os.path.basename(shader_file) + ".spv"
    spv_path = os.path.join(BUILD_OUTPUT_DIR, spv_name)
    result = subprocess.run([
        "glslangValidator",
        "-V", shader_file,
        "-o", spv_path,
    ], capture_output=True, text=True)

    if result.returncode != 0:
        print(f"[ERROR] Failed to compile shader {shader_file}")
        print(result.stdout)
    else:
        print(f"[OK] Compiled shader {shader_file}")

def main():
    for root, _ , files in os.walk(SHADER_DIR):
        for file in files:
            if any(file.endswith(ext) for ext in EXTENSIONS):
                compile_shader(os.path.join(root, file))


    with open(output_flag, "w") as f:
        f.write("OK\n")

if __name__ == "__main__":
    main()