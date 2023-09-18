# Cerberus
## Description
### A C++ tool to unstrip Rust and Go binaries (ELF and PE) 
**Cerberus** is the tool you want to use to make RUST and GO static analysis a lot easier.  
Based on hashing and scoring systems, it can retrieve lots of symbol names.
## How does it work ?
After analyzing your ELF/PE binary to find the used libraries, **Cerberus** will download and build them.  
Then the tool will hash (in various ways) the functions in your file and in the libraries to make matches.  
## Table of contents
[Installation](#install)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Download a release](#install_release)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Build the tool with Docker](#install_build_docker)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Build the tool on host](#install_build_host)  
[How to use ?](#how)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Syntax](#how_syntax)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Parameters](#how_params)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Flags](#how_flags)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Example](#how_example)  
[Warning](#warning)  

<a name="install"/>

## Installation

<a name="install_release"/>

### Download a release
Check the [Releases](https://github.com/h311d1n3r/Cerberus/releases/) tab on the Github project and download the latest one.  

<a name="install_build_docker"/>

### Build the tool with Docker
1. Clone the repository `git clone https://github.com/h311d1n3r/Cerberus && cd cerberus`.
2. Check the available Dockerfiles under `Cerberus/docker/{OS}`.  
3. Build the docker image of your choice `docker build -f ./docker/{OS}/Dockerfile-{version}`.
4. You can run **Cerberus** from inside the docker or extract the binary on your host. This second choice needs to install the libraries listed in [this section](#install_build_host).  

<a name="install_build_host"/>

### Build the tool on host  
1. You need to have **libarchive**, **libcurl4-openssl**, **zlib1g**, **libelf** and the **uuid-dev** libraries installed on your system.  
With APT just do `apt -y install libarchive-dev libcurl4-openssl-dev zlib1g-dev libelf-dev`
2. Clone the repository `git clone https://github.com/h311d1n3r/Cerberus && cd cerberus`.  
3. Create the build directory `mkdir build && cd build`.  
4. Run CMake to configure the project `cmake ..`.
5. Run make to compile the project `make`.  

<a name="how"/>

## How to use ?

<a name="how_syntax"/>

### Syntax
`cerberus binary [-param value] [--flag]`

<a name="how_params"/>

### Parameters
`output` -> Specifies the path for the resulting ELF file.  
`part_hash_len` -> Specifies the length of a `part hash`. The `part hash` of a function is just a reduction of the function with a linear pace.
This technique is used to prevent fixed addresses from corrupting a standard hash. Default value : 20  
`part_hash_trust` -> Specifies minimum ratio of similarity between the two hashed functions to compare. The kept function will be the one with the most matches anyway.
Increasing this value will reduce the number of matched functions but speed up execution time. Default value : 0.6  
`min_func_size` -> The minimum length a function must be to get analyzed. Decreasing this value will increase matches but also false positives. Default value : 10  

<a name="how_flags"/>

### Flags
`help` -> Displays a help message.  
`debug` -> Displays outputs of commands.  
`no-prompt` -> Automatically skips user prompts.  

<a name="how_example"/>

### Example
#### Command
The following command will try to unstrip the file ./rust_example into a new ELF called ./rust_example_syms.  
`cerberus ./rust_example -output ./rust_example_syms`
#### Result
Here is a comparison of the main function in the two files using Binary Ninja :  

<p align="center">
  <img src="https://i.imgur.com/uvpC63E.png" alt="before.png"/>
</p>

<p align="center">
  <img src="https://i.imgur.com/Sp3ct49.png" alt="after.png"/>
</p>

<a name="warning"/>

## Warning
**This software must only be used to carry out lawful experiments and I am not responsible for any breach of this rule !**  
