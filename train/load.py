import torch 
import json

m = torch.jit.load("model.pth")
dic = {}
count = 0
for parameter in m.parameters():
    if(count%2 == 0):
        dic['features.'+str(count // 2)+'.weight'] = parameter.detach().numpy().tolist()
        count += 1
    else:
        dic['features.'+str(count // 2)+'.bias'] = parameter.detach().numpy().tolist()
        count += 1


with open("model.json",'w') as file:
    json.dump(dic,file)