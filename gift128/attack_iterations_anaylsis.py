from wrapper import KeyRecovery, oracleSimulator  
import numpy as np
from json import load
import random 

iters=100
n_faults_req=[0 for _ in range(iters)]

def testAttack():
    EXP = 16
    key = np.random.randint(0, 16, size=(32), dtype=np.uint8)
    print(key)
    cips, fcips = oracleSimulator(key, EXP)
    remainingKeys = KeyRecovery(cips, fcips, key, EXP)

    print("Remaining Keys ", remainingKeys)

def attack(orcips, orfcips, key,iterations):
    EXP = len(orcips)
    key = np.asarray(key, dtype=np.uint8)
    cips = []
    fcips = []
    for c in range(0,5):
        cips += orcips[c]
        fcips += orfcips[c]
    
    for test_n_faults in range(5,EXP):
        print(" ------- ", test_n_faults+1, " ------- ")
        cips += orcips[test_n_faults]
        fcips += orfcips[test_n_faults]
			
        cips1 = np.asarray(cips, dtype=np.uint8)
        fcips1 = np.asarray(fcips, dtype=np.uint8)
        remainingKeys = KeyRecovery(cips1, fcips1, key, test_n_faults+1)
        if(remainingKeys==1):
            print("------ Remaining Keys ", remainingKeys, " in faults ", test_n_faults+1)
            n_faults_req[iterations]=test_n_faults+1
            return
        print(" ------- INSUFFICIENT ------- ",remainingKeys)

if __name__ == '__main__':

	num_fauts=12
	
	key = [0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF, 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF]
	plaintext = [0 for _ in range(num_fauts)]
	
	
	
	for iterations in range(iters):
		
		for nf in range(0,num_fauts):
			plaintext[nf]=bytearray([random.randint(0,15) for _ in range(32)]) #(_+nf)%16 
        
		pt_list=[]
		cips=[]
		fcips=[]
		
		filename = 'TOFA_FA.ipynb'
		with open(filename) as fp:
			nb = load(fp)
		for cell in nb['cells']:
			if cell['cell_type'] == 'code':
				source = ''.join(line for line in cell['source'] if not line.startswith('%'))
				exec(source, globals(), locals())
		print("\n\n\nKey Recovery Starts now----------")
	    
		fp.close()
		#print(pt_list,cips,fcips)
	
		attack(cips, fcips, key,iterations)

		print(n_faults_req)
	
    
    
    
