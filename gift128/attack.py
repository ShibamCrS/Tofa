from wrapper import KeyRecovery, oracleSimulator  
import numpy as np
from json import load

def testAttack():
    EXP = 16
    key = np.random.randint(0, 16, size=(32), dtype=np.uint8)
    print(key)
    cips, fcips = oracleSimulator(key, EXP)
    remainingKeys = KeyRecovery(cips, fcips, key, EXP)

    print("Remaining Keys ", remainingKeys)

def attack(orcips, orfcips, key):
    EXP = len(orcips)
    key = np.asarray(key, dtype=np.uint8)

    cips = []
    fcips = []
    for c in orcips:
        cips += c
    for c in orfcips:
        fcips += c
    cips = np.asarray(cips, dtype=np.uint8)
    fcips = np.asarray(fcips, dtype=np.uint8)
    remainingKeys = KeyRecovery(cips, fcips, key, EXP)
    print("Remaining Keys ", remainingKeys)

if __name__ == '__main__':

	num_fauts=7
	
	key = [0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF, 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF]
	filename = 'TOFA_FA.ipynb'
	with open(filename) as fp:
		nb = load(fp)

	for cell in nb['cells']:
		if cell['cell_type'] == 'code':
		    source = ''.join(line for line in cell['source'] if not line.startswith('%'))
		    exec(source, globals(), locals())
	print("\n\n\nKey Recovery Starts now----------")
	    
	attack(cips, fcips, key)
	
    
    
    
