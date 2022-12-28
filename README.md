# Cerberus
## Description
### A Python tool to unstrip Rust binaries on Linux
**Cerberus** is the tool you want to use to make Rust static analysis a lot easier.  
Based on hashing and scoring systems, it can retrieve lots of symbol names.
## How does it work ?
After analyzing your ELF binary to find the used crates, **Cerberus** will download and build them.  
Then the tool will hash (in various ways) the functions in your file and in the crates to make matches.  
## Table of contents
[Build the tool](#build)  
[How to use ?](#how)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Syntax](#how_syntax)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Parameters](#how_params)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Flags](#how_flags)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Example](#how_example)  
[Warning](#warning)  

<a name="build"/>

## Build the tool  
1. You need to have python3, cargo and the binutils package installed on your system.  
2. Install pyinstaller and lief using `pip3 install package_name`.  
3. Clone the repository.  
4. Build the tool using `pyinstaller src/cerberus.py src/*.py --onefile --clean`  
5. Add the generated `dist` directory to your path using `PATH=$PATH:~/path/to/the/repo/dist`.  

<a name="how"/>

## How to use ?

<a name="how_syntax"/>

### Syntax
`cerberus binary [-param value] [--flag]`

<a name="how_params"/>

### Parameters
`output` -> Specifies the path for the resulting ELF file.  

<a name="how_flags"/>

### Flags
`help` -> Displays a help message.  

<a name="how_example"/>

### Example
#### Command
The following command will try to unstrip the file ./rust_example into a new ELF called ./rust_example_syms.  
`cerberus ./rust_example -output ./rust_example_syms`
#### Result
Here is a comparison of the main function in the two files using Binary Ninja :  

<p align="center">
  <img src="https://i.imgur.com/NjsspzI.png" alt="before.png"/>
</p>

<p align="center">
  <img src="https://i.imgur.com/Sp3ct49.png" alt="after.png"/>
</p>

<a name="warning"/>

## Warning
**This software must only be used to carry out lawful experiments and I am not responsible for any breach of this rule !**  
