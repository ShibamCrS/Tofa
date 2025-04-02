import numpy as np
from json import load
import random 

iters=100



if __name__ == '__main__':

	num_fauts=13
	
	key = [0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF, 0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF]
	plaintext = [0 for _ in range(num_fauts)]
	
	
	
	for iterations in range(12,iters):
		
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
		filename1 = "cips_"+str(iterations)
		with open(r'./DATA/'+filename1+'.txt', 'w') as fp1:
			for c in cips:
				for j in c:
					fp1.write("%s " % j)
				fp1.write("\n")
		fp1.close()
		filename2 = "fcips_"+str(iterations)
		with open(r'./DATA/'+filename2+'.txt', 'w') as fp2:
			for c in fcips:
				for j in c:
					fp2.write("%s " % j)
				fp2.write("\n")
		fp2.close()
		print('Done')
	
    
    
    
