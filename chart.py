import matplotlib.pyplot as plt
import csv
x=[]
y=[]   
fig = plt.figure()
f= open("time.csv","r")

#append values to list
for row in csv.reader(f):
    x.append(int(row[0]))
    y.append(float(row[1]))

print(x)
print(y)
plt.plot(x, y, 'r.')
fig.savefig("chart.jpg")