import sys
import random
import math
args = sys.argv
if(len(args) != 12  ):
  print "Invalid number of arguments"
  exit()

ax = float(args[1])
ay = float(args[2])

bx = float(args[3])
by = float(args[4])

cx = float(args[5])
cy = float(args[6])

initialX = float(args[7])
initialY = float(args[8])

count = int(args[9])

tupleFileName = args[10]
averageFileName = args[11]

tupleFile = open(tupleFileName, 'w')
averageFile = open(averageFileName, 'w')
avgX = 0 
avgY = 0
tupleFile.write('%.3f %.3f %.3f %.3f %.3f %.3f\n' % (ax, ay, bx, by, cx, cy))
print ax, ay, bx, by, cx, cy
for x in range(count):
  da = math.sqrt(pow(initialX - ax, 2)  + pow(initialY - ay, 2))
  db = math.sqrt(pow(initialX - bx, 2)  + pow(initialY - by, 2))
  dc = math.sqrt(pow(initialX - cx, 2)  + pow(initialY - cy, 2))
  da = round(da, 3)
  db = round(db, 3)
  dc = round(dc, 3)
  tupleFile.write('%.3f %.3f %.3f\n' % (da, db, dc))
  #averageFile.write('%.3f %.3f\n' % (initialX, initialY)) 
  avgX += initialX
  avgY += initialY 
  if((x + 1 )% 4 == 0):
    avgX = avgX / 4
    avgY = avgY/ 4
    averageFile.write('%.3f %.3f\n' % (avgX, avgY))
    avgX = 0
    avgY = 0
  
  delX = random.uniform(1, 10);
  delY = random.uniform(1, 10);
  delX = round(delX, 3)
  delY = round(delY, 3)
  if(random.randint(0, 10) % 2 == 0):
    initialX += delX
  else:
    initialX -= delX
  if(random.randint(0, 10) % 2 == 0):
    initialY += delY
  else:
    initialY -= delY

tupleFile.close()
averageFile.close()