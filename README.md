
# C++ Project Template

This is a minimal C++ project for testing development in VS Code. The project name and executable name are set via `project_config.txt` in the project root.

## Configuration

Edit `project_config.txt` to set the project name and executable name:

```
PROJECT_NAME=project1
```

## Build Instructions

### Using Conan (Recommended)

1. Install Conan if you haven't already:
   ```sh
   pip install conan
   # or
   brew install conan
   ```


2. Build the project:
   ```sh
   ./scripts/build.sh
   ```

## Usage

Run the executable using the provided script:

```sh
./scripts/run.sh
```

To clean build artifacts:

```sh
./scripts/clean.sh
```

You should see:
```
Hello, world!
```

## OpenMP Support on macOS
To enable OpenMP with Apple Clang, install libomp:

```sh
brew install libomp
```

When compiling with OpenMP, use:

```sh
clang++ -Xpreprocessor -fopenmp -lomp your_file.cpp -o your_program
```
