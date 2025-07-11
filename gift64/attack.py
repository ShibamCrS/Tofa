from wrapper import KeyRecovery, oracleSimulator  
import numpy as np
from json import load


num_fauts=30


def testAttack():
    EXP = num_fauts
    key = np.random.randint(0, high=np.iinfo(np.uint64).max, size=2, dtype=np.uint64)
    print(key)
    cips, fcips = oracleSimulator(key, EXP)
    # print(cips, fcips)
    remainingKeys = KeyRecovery(cips, fcips, key, EXP)

    print("Remaining Keys ", remainingKeys)

def attack(orcips, orfcips, key):
    EXP = len(orfcips)
    key = np.asarray(key, dtype=np.uint64)

    cips = np.asarray(orcips, dtype=np.uint64)
    fcips = np.asarray(orfcips, dtype=np.uint64)
    remainingKeys = KeyRecovery(cips, fcips, key, EXP)
    #print("Remaining Keys ", remainingKeys)

if __name__ == '__main__':
    

    key = [0x0123456789ABCDEF, 0x0123456789ABCDEF]
    
    filename = './TOFA_FA.ipynb'
    with open(filename) as fp:
        nb = load(fp)
    for cell in nb['cells']:
        if cell['cell_type'] == 'code':
            source = ''.join(line for line in cell['source'] if not line.startswith('%'))
            exec(source, globals(), locals())
    print("\n\n\nKey Recovery Starts now----------")
    
    cips_word=[]
    fcips_word=[]
    
    
    
    for i in range(num_fauts):
        cips_word.append(0)
        fcips_word.append(0)
        for j in range(16):
            cips_word[i]=((cips[i][j]&0xF)<<(4*j))+cips_word[i]
            fcips_word[i]=((fcips[i][j]&0xF)<<(4*j))+fcips_word[i]

       
    attack(cips_word, fcips_word, key)
    testAttack()

    
    
    
