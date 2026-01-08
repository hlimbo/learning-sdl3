## Quickstart using CMake 3.16

### Build SDL3 from source - Windows
1. Configure CMake
```bash
cmake -S . -B build
```

2. Build CMake Project
```bash
cmake --build build
```

3. If build is successful, you should find `SDL3.dll` located in
```
vendored/SDL/build/debug/SDL3.dll
```


## Commands to run when in the `vendored/SDL` directory on command line

### Build `vendored/SDL` folder with Examples on Windows
```bash
cmake -S . -B build -D SDL_EXAMPLES=ON
cmake --build build
```

* If build is good, you should find the `.exe` files in
```
`vendored/SDL/build/examples/Debug`
```