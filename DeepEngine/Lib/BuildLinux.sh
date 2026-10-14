#!/bin/sh

clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Mesh.cpp -o Mesh.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Shader.cpp -o Shader.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/ShadowMap.cpp -o ShadowMap.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Texture.cpp -o Texture.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Window.cpp -o Window.o
ar rcs libDeepEngineStatic.a Mesh.o Shader.o ShadowMap.o Texture.o Window.o
rm -rf Mesh.o
rm -rf Shader.o
rm -rf ShadowMap.o
rm -rf Texture.o
rm -rf Window.o

clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Mesh.cpp -o Mesh.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Shader.cpp -o Shader.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/ShadowMap.cpp -o ShadowMap.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Texture.cpp -o Texture.o
clang++ --target=x86_64-llvm-linux -std=c++17 -I../Include -O3 -fPIC -c ../Source/Window.cpp -o Window.o
clang++ --target=x86_64-llvm-linux -shared Mesh.o Shader.o ShadowMap.o Texture.o Window.o -o libDeepEngine.so
rm -rf Mesh.o
rm -rf Shader.o
rm -rf ShadowMap.o
rm -rf Texture.o
rm -rf Window.o
