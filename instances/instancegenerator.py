import random

n = 0
K = 0
RD = []
VC = []
RT = []
ALE = []
ALL = []

random.seed()

def frange(start, stop, step):
	i = start
	while i < stop:
		yield i
		i += step


def generator(ln, un, lk, uk):
	for i in range(ln, un):
		for k in range(lk, uk):
			VC = random.sample(range(3, 11), k)
			RD = random.sample(range(1, max(VC)), i)
			
			X  = [random.uniform(1,40) for _ in range(i*2)]
			Y  = [random.uniform(1,40) for _ in range(i*2)]
			RT = [random.uniform(1,60) for _ in range(i)]
			ALE = [random.uniform(1,120) for _ in range(i)]
			ALL = [random.uniform(1,120) for _ in range(i)]

			print (i)
			print (k)
			print (RD)
			print (VC)

			print (X)
			print (Y)
			print (RT)
			print (ALE)
			print (ALL)
			print("\n*******\n")



generator(5, 7, 3, 5)