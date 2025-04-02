





# HOW TO INDUCE THE FAULTS AND PERFORM KEY RECOVERY?
The following two commands are sufficient to build the necessary dependencies, induce faults, and perform final key recovery.

> $ python setup.py build_ext --inplace 
> $ python3 attack.py


Hardware: The GIFT-128 packed implementation is run on 8-bit Harvard architecture-based microcontroller ATXmega128D4-AU on the ChipWhisperer-Lite (CW1173) evaluation board (https://rtfm.newae.com/Targets/CW303\%20XMEGA/). 

ToolChain: To mount the attack on the device, we use the ChipWhisperer open-source toolchain. Please ensure this toolchain is set up before proceeding with the attack. The folder 'Setup_Scripts' and the 'TOFA' folder pertain to this toolchain's requirements. The contents of the TOFA folder build the necessary .hex files required for programming the microcontroller. To do this, run a 'make' command in the TOFA folder.

Fault Injection: The script used to induce the fault is TOFA_FA.ipynb. This Jupyter Notebook runs the GIFT128 encryption and injects faults via clock glitching. It can be opened and run to see how this is done. Note that the script will only run in the presence of the hardware (CW1173). 

Key-Recovery: The Jupyter Notebook- TOFA_FA.ipynb is called via Python script- attack.py. This Python file collects the faulty and correct ciphertexts and returns the number of keys recovered. In the parameters set, a unique key is obtained. The parameters within this file responsible for the number of faults 'num\_faults' can be modified to see how the key recovery works for fewer faults.

# COUNTERMEASURE

We provide a duplication-based countermeasure implementation of GIFT128 for the last five rounds required to mitigate TOFA. It is present in the folder- Countermeasure_TOFA. A test of this countermeasure is shown in the Jupyter Notebook file- TOFA_FA_Countermeasure.ipynb. Any fault induced here results in all 0 ciphertext results. It prevents the attacker from exploiting faults on GIFT128.


