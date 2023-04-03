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
1. You need to have **Python3**, **Cargo** and the **binutils** package installed on your system.  
2. Clone the repository.  
3. Install Python dependencies using `pip3 install -r requirements.txt`.  
4. Build the tool using `pyinstaller src/cerberus.py src/*.py --onefile --clean`.  
5. Add the generated `dist` directory to your path using `PATH=$PATH:~/path/to/the/repo/dist`.  

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
`debug` -> Enable debug level of logging.  

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
