## Quickstart using CMake 3.16

### Prereqs
1. Install XCode and verify that the cmake command below uses clang as its compiler by running the `Configure CMake` command below (It should be the first 2 lines when it starts the configuration process)

Example Output:
```
-- The C compiler identification is AppleClang 17.0.0.17000013
-- The CXX compiler identification is AppleClang 17.0.0.17000013
```

### Build SDL3 from source - Mac
1. Configure CMake
```bash
cmake -S . -B build
```

2. Build CMake Project
```bash
cmake --build build
```

3. If the build is good, you should see the app in:
```
mac/build/hello-mac.app
```

Can run the project by opening the `build` folder in Finder and double clicking on `hello-mac` to open the hello world program

## Commands to run when in the `vendored/SDL` directory on command line

### Build `vendored/SDL` folder with Examples on Windows
```bash
cmake -S . -B build -D SDL_EXAMPLES=ON
cmake --build build
```