[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![MIT][license-shield]][license-url]



<!-- PROJECT LOGO -->
<div align="center">

<h3 align="center">3D Hydrogen Electron Cloud</h3>

  <p align="center">
    3D Visualization of a Hydrogens' Electron Cloud
  </p>

</div>

<!-- ABOUT THE PROJECT -->
## About The Project
Direct calculation of the probability density of a Hydrogen Atoms' Wave function  
Currently there is only a CPU-side calculation, with GPGPU-Oriented approaches coming soon :)

### Pretty Pictures
![Hydrogen Electron cloud for quantum numbers N, L, M = 2, 1, 1](https://github.com/inonitz/cmake_cuda_physics/tree/main/electron_prob_cloud_nlm_210_inferno_colourmap.png)
<br></br>

### Project Structure
The project structure is the same as my [premake5-workspace-template](https://github.com/inonitz/premake5-workspace-template), except that I use CMake instead  
<br></br>

### Built With


[<img width="200" height="100" src="https://cmake.org/wp-content/uploads/2023/08/CMake-Logo.svg">](https://cmake.org)  
[![SDL3][SDL3.js]][SDL3-url]

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

* CMake
* Working compiler toolchain, preferably clang
  * Windows: You should use [llvm](https://github.com/llvm/llvm-project/releases)
  * Linux:
      1. [installing-specific-llvm-version](https://askubuntu.com/questions/1508260/how-do-i-install-clang-18-on-ubuntu)
      2. [configuring-symlinks](https://unix.stackexchange.com/questions/596226/how-to-change-clang-10-llvm-10-etc-to-clang-llvm-etc)
      3. **You Don't have to use LLVM, gcc works too**

### Installation
```sh
git clone --recurse-submodules https://github.com/inonitz/cmake_cuda_physics/cmake_cuda_physics.git
cd cmake_cuda_physics
```
<br></br>

<!-- USAGE EXAMPLES -->
## Usage

### Build Process

```sh
./build.sh debug/release/release_dbginfo static/shared clean/noclean
```

<br></br>

<!-- ROADMAP -->
## Roadmap

* Add ImGui for dynamic images
* Use SDL3/CUDA for 3D Visualization of the electron cloud
* Use/Develop an Equation Solver, instead of hardcoding result

<!-- CONTRIBUTING -->
## Contributing

If you have a suggestion, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".  

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` file for more information.

<!-- ACKNOWLEDGEMENTS -->
<!-- ## Acknowledgement -->

<!-- References -->
## References

* [Tutorial for Displaying Electron Probability Cloud in Hydrogen Atom](medium.com/@apsandiwan93/electron-probability-cloud-in-hydrogen-atom-c8702bd24d01)
* [Modern CMake](cliutils.gitlab.io/modern-cmake/README.html)
* [Color Advice](https://www.kennethmoreland.com/color-advice)
* [Best-README](https://github.com/othneildrew/Best-README-Template)




<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/inonitz/cmake_cuda_physics?style=for-the-badge&color=blue
[contributors-url]: https://github.com/inonitz/cmake_cuda_physics/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/inonitz/cmake_cuda_physics?style=for-the-badge&color=blue
[forks-url]: https://github.com/inonitz/cmake_cuda_physics/network/members
[stars-shield]: https://img.shields.io/github/stars/inonitz/cmake_cuda_physics?style=for-the-badge&color=blue
[stars-url]: https://github.com/inonitz/cmake_cuda_physics/stargazers
[issues-shield]: https://img.shields.io/github/issues/inonitz/cmake_cuda_physics.svg?style=for-the-badge
[issues-url]: https://github.com/inonitz/cmake_cuda_physics/issues
[license-shield]: https://img.shields.io/github/license/inonitz/cmake_cuda_physics?style=for-the-badge
[license-url]: https://github.com/inonitz/cmake_cuda_physics/blob/main/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/linkedin_username

[SDL3-url]: https://github.com/libsdl-org
[SDL3.js]: https://libsdl.org/media/SDL_logo.png