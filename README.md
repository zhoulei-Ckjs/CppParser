# CppParser
利用LLVM的clang来解析c++文件，提取类、函数注释

# LLVM安装
llvm版本：llvmorg-18.1.8
```
git clone git@github.com:llvm/llvm-project.git
cmake -S llvm -B build -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -DLLVM_INSTALL_UTILS=ON -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_EH=ON
#选项解释
#-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"  我们需要用到clang前端编译器
#-DLLVM_INSTALL_UTILS=ON
#-DLLVM_ENABLE_RTTI=ON    启用C++运行时类型识别RTTI
#-DLLVM_ENABLE_EH=ON      启用exception
cmake --build build
cmake --install build
```

# 此项目编译运行
```angular2html
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build
./FunctionParser ../example/example.h
```
