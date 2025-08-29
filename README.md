# Hello C++ Project

This is a minimal C++ project for testing development in VS Code.


## Build Instructions

### Using Conan (Recommended)

1. Install Conan if you haven't already:
   ```sh
   pip install conan
   # or
   brew install conan
   ```

2. Install dependencies and generate toolchain files:
   ```sh
   conan install . --output-folder=build --build=missing
   ```

3. Build the project:
   ```sh
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
   cmake --build .
   ```

4. Run the executable:
   ```sh
   ./hello
   ```

You should see:
```
Hello, world!
```
