import numpy as np

A = [[5,1,2,4,2],
     [2,3,4,1,1],
     [2,1,1,3,1],
     [2,2,1,3,5],
     [0,2,2,3,1]]

print("Rechteck:")
[print(x) for x in A]

print("Rechteck C_sat:")
Asat = np.cumsum(A,axis=0).cumsum(axis=1)
print(Asat)



print("schraffiertes Rechteck:")
B = [[1,2,4,2],[3,4,1,1]]
[print(x) for x in B]

print("schraffiertes Rechteck C_avg:")
i0 = 0
j0 = 1
i1 = 1
j1 = 4
lower = (i1 - i0 + 1 )*(j1 - j0 + 1)

# not sure about this:
r1 = Asat[i1][j1] # 25
r2 = 0 #Asat[i0-1][j1] # out of index, daher 0?
r3 = Asat[i1][j0-1] # 11
r4 = 0 #Asat[i0-1][j0-1] # out of index, daher 0?
print("(",r1,"-",r2,"-",r3,"+",r4,")/",lower)

upper = (r1 - r2 - r3 + r4)
print(str(upper) + " / " + str(lower) + " = " + str(upper/lower))

counter = 0
for i in range(i0,i1+1):
    for j in range(j0,j1+1):
        counter = counter + 1

print("we would need " + str(counter) + " zugriffe with the standart formula")

# r1 + r3
print("we need 2 zugriffe with the c_avg formula")