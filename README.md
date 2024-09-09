# CppParser
利用LLVM的clang来解析c++文件，提取类、函数注释

# LLVM安装
llvm版本：llvmorg-18.1.8

克隆
```shell
git clone --recurse-sudmodules --branch llvmorg-18.1.8 --depth 1 git@github.com:llvm/llvm-project.git
```

编译Linux
```shell
cmake -S llvm -B build -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -DLLVM_INSTALL_UTILS=ON -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_EH=ON -DCMAKE_INSTALL_PREFIX=./lib
#选项解释
#-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"  我们需要用到clang前端编译器
#-DLLVM_INSTALL_UTILS=ON
#-DLLVM_ENABLE_RTTI=ON    启用C++运行时类型识别RTTI
#-DLLVM_ENABLE_EH=ON      启用exception
cmake --build build
cmake --install build
```

编译Windows
```shell
cmake -G Ninja -S llvm -B build -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -DLLVM_INSTALL_UTILS=ON -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_EH=ON -DCMAKE_CXX_FLAGS="/utf-8" -DLLVM_BUILD_LLVM_DYLIB=OFF -DLLVM_LINK_LLVM_DYLIB=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DLLVM_TARGETS_TO_BUILD="X86" -DCMAKE_INSTALL_PREFIX=../slib
ninja -C build install
```

# CppParser编译运行

Linux编译
```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=../lib
cmake --build build
./build/FunctionParser example output.json
```

Windows编译
```shell
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=../slib -DCMAKE_CXX_FLAGS="/utf-8"
ninja -C build
FunctionParser example output.json
```
