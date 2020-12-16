import csv
import numpy
import pandas as pd
import math

date = ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15"]


raidus = [0.019667478, 0.029940668, 0.040076348, 0.0611749, 0.080543666, 0.091776206, 0.11, 0.133019267]


bottomLat = 30.524081949676
topLat = 30.7938780503239
leftLon = 103.908407474531
rightLon = 104.222044525468
startTime = 1477929600
endTime = startTime + 1296000
midLat = (bottomLat + topLat) / 2
midLon = (leftLon + rightLon) / 2

data = []
label = []

maxTime = 40 * 60

count = 0
for d in date:
    for part in range(0, 5):
        filename = "order_201611" + d + "_part" + str(part) + ".csv"
        with open(filename) as f:
            reader = csv.reader(f)
            head = next(reader)
            for entry in reader:
                vec = []
                depatureTime = int(entry[1])
                arriveTime = int(entry[2])
                origLng = float(entry[3])
                origLat = float(entry[4])
                destLng = float(entry[5])
                destLat = float(entry[6])
                if depatureTime < startTime or depatureTime >= endTime:
                    continue
                if arriveTime < startTime or arriveTime >= endTime:
                    continue
                if origLat < bottomLat or origLat > topLat:
                    continue
                if origLng < leftLon or origLng > rightLon:
                    continue
                if destLat < bottomLat or destLat > topLat:
                    continue
                if destLng < leftLon or destLng > rightLon:
                    continue

                vec.append((depatureTime - startTime) % 86400)
                label.append((arriveTime - depatureTime))
                day = int(d)
                day = (day + 1) % 7
                if day == 6 or day == 0:
                    vec.extend([0, 1])
                else:
                    vec.extend([1, 0])

                vec.append(origLng - leftLon)
                vec.append(origLat - bottomLat)
                r = math.sqrt((origLat - midLat) * (origLat - midLat) + (origLng - midLon) * (origLng - midLon))
                tmp = 0
                for t in range(0, len(raidus)):
                    if r < raidus[t]:
                        break
                    else:
                        tmp += 1
                vec.extend([0] * (tmp))
                vec.append(1)
                vec.extend([0] * (len(raidus) - tmp))
                
                vec.append(destLng - leftLon)
                vec.append(destLat - bottomLat)
                r = math.sqrt((destLat - midLat) * (destLat - midLat) + (destLng - midLon) * (destLng - midLon))
                tmp = 0
                for t in range(0, len(raidus)):
                    if r < raidus[t]:
                        tmp += 1
                vec.extend([0] * tmp)
                vec.append(1)
                vec.extend([0] * (len(raidus) - tmp))
                
                data.append(vec)
                count += 1
                if (count % 100000 == 0):
                    print(len(data))
                
            
df = pd.DataFrame(data)
dfl = pd.Series(label)
des = df.describe()
print(des)
des.to_csv("data_des.csv")


for i in [0, 3, 4, 14, 15]:
    if des[i]['std'] != 0:
        df[i] = df[i].map(lambda x: (x - des[i]['mean']) / des[i]['std'])
        print(i)

dfl = (dfl - dfl.mean()) / dfl.std()

df.to_csv("data.csv")
dfl.to_csv("label.csv")
