---

Title: "ToFA: Towards Fault Analysis of GIFT and GIFT-like Ciphers Leveraging Truncated Impossible Differentials"
Implementation: Shibam Ghosh and Aikata Aikata and Anup Kumar Kundu
Published At: "Transactions on Cryptographic Hardware and Embedded Systems (CHES) Volume 2025 Issue 3"
Full Version:  " https://eprint.iacr.org/2024/1927 "

---
This repository provides supplementary material for our work on the fault analysis of **GIFT** and **GIFT-like** ciphers leveraging truncated impossible differentials.
=========================================================

<p align="center">
  <img src="fig/tofa.png" />
</p>

## Table of contents

1. [Software Simulation of The Attacks](#simulation)
1. [Attack Realization Using ChipWhisperer Lite](#fault)
&nbsp;

## Software Simulation of The Attacks
The simulation code is given in Gift128/64 and BAKSHEESH are given in respective directories.
To run the simulation, do the following:
```bash
cd cipher_name
make
./byte_fault
```
To change number of Faults, set #define EXP (e.g., for 16 faults #define EXP 16)

## Attack Realization Using ChipWhisperer Lite

We use the ChipWhisperer-Lite  CW1173  evaluation board, as shown below. It has XMEGA and ARM targets as shown in `./setup.jpeg`.

 <!-- <div  align="center"><img  src="./setup.jpeg"  width="400"  height="200"></div> -->


## Dependencies

We use the ChipWhisperer open-source toolchain (https://github.com/newaetech/chipwhisperer). Hence, this toolchain must be installed to replicate the attacks on the hardware utilized in this work.

For the simulated key recoveries, C/C++ compilers and interpreters are sufficient.



# HOW TO INDUCE THE FAULTS AND PERFORM KEY RECOVERY?
The following two commands are sufficient to build the necessary dependencies, induce faults, and perform final key recovery in the `Tofa/gift<128/64>` folder.

> $ python setup.py build_ext --inplace
> 
> $ python3 attack.py

Each folder contains `Tofa/gift<128/64>/sample_output.txt` file to give an impression of what the output must look like. Overall, it tells whether the original key was found or not. Sometimes this can be unique, other times, it is present in a small set of possible keys. `-1` implies that the key could not be found. If this is the case, then the glitch setup must be revised. This involves varying the following glitch parameters in `Tofa/gift<128/64>/TOFA_FA.ipynb` file: 1) `scope.glitch.repeat`, 2) `scope.glitch.width`, and 3) `scope.glitch.offset`. We obtained these experimentally by checking how many faults it initially induces.


Hardware: The GIFT-128 packed implementation is run on 8-bit Harvard architecture-based microcontroller ATXmega128D4-AU on the ChipWhisperer-Lite (CW1173) evaluation board (https://rtfm.newae.com/Targets/CW303\%20XMEGA/). 

### ToolChain 
To mount the attack on the device, we use the ChipWhisperer open-source toolchain. Please ensure this toolchain is set up before proceeding with the attack. We use `Python 3.12.3` for our experiments. The folder `Setup_Scripts` and the `TOFA` folder pertain to this toolchain's requirements. Installing Chipwhisperer (preferably using the open-source git toolchain) and the ./requirements.txt file is sufficient. The contents of the TOFA folder build the necessary `.hex` files required for programming the microcontroller. To do this, run a 'make' command in the TOFA folder.

### Fault Injection 
The script used to induce the fault is `TOFA_FA.ipynb`. This Jupyter Notebook runs the GIFT64/128 encryption and injects faults via clock glitching. It can be opened and run to see how this is done. Note that the script will only run in the presence of the hardware (CW1173). 

### Key-Recovery 
The Jupyter Notebook- `TOFA_FA.ipynb` is called via Python script- attack.py. This Python file collects the faulty and correct ciphertexts and returns the number of keys recovered. In the parameters set, a unique key is obtained. The parameters within this file responsible for the number of faults `num\_faults` can be modified to see how the key recovery works for fewer faults.

# COUNTERMEASURE

We provide a duplication-based countermeasure implementation of GIFT128 for the last five rounds required to mitigate TOFA. It is present in the folder- `Tofa/gift128/Countermeasure_TOFA/`. A test of this countermeasure is shown in the Jupyter Notebook file- `TOFA_FA_Countermeasure.ipynb`. Any fault induced here results in all 0 ciphertext results. It prevents the attacker from exploiting faults on GIFT128.


## License

TOFA is licensed under the BSD 2-Clause License. See [LICENSE](LICENSE) for more details.

<br />
<br />
