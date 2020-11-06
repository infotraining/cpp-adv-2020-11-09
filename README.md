# Szkolenie - Zaawansowane programowanie w C++ #

## Docs

* https://infotraining.bitbucket.io/cpp-adv
* https://gitpitch.com/infotraining-dev/cpp-adv/develop?grs=bitbucket&p=slides_pl#/

## Toolchain

### Kompilator + CMake

* Dowolny kompilator C++ (gcc, clang, Visual C++) wspierający C++11/14

* [CMake > 3.8](https://cmake.org/)

### [Menadżer pakietów - Vcpkg (opcjonalnie)](https://github.com/microsoft/vcpkg)
  
  #### Do pobrania

  * https://github.com/microsoft/vcpkg

  #### Instalacja (Linux)

  ```
  $cd ~
  
  $ git clone https://github.com/microsoft/vcpkg
  $ ./vcpkg/bootstrap-vcpkg.sh
  ```

  Dodać do pliku `.profile`:

  ```
  export VCPKG_ROOT=$HOME/vcpkg
  ```
  
  Instalacja bibliotek:

  ```
  $ ./vcpkg/vcpkg install [packages to install]
  ```

## Biblioteki

* [Catch2](https://github.com/catchorg/Catch2)

## Visual Studio 2019

### Plugins

* Live Share
