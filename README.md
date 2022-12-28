# Cerberus
## Description
### A Python tool to unstrip Rust binaries
**Cerberus** is the tool you want to use to make Rust static analysis a lot easier.  
Based on hashing and scoring systems, it can retrieve lots of symbol names.
## How does it work ?
After analyzing your ELF binary to find the used crates, **Cerberus** will download and build them.  
Then the tool will hash (in various ways) the functions in your file and in the crates to make matches.  
## Table of contents
[Warning](#warning)  

<a name="warning"/>

## Warning
**This software must only be used to carry out lawful experiments and I am not responsible for any breach of this rule !**  
