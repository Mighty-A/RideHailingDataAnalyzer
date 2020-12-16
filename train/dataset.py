from torch.utils.data import Dataset, Subset, sampler, DataLoader
import torch
import pandas as pd
import numpy as np

validation_split = 0.2
shuffle_dataset = True
random_seed = 44
batch_size = 16384

if torch.cuda.is_available():
    device = torch.device('cuda')
else:
    print("no gpu")


    # read in data
class RideHailingDataset(Dataset):
    def __init__(self, csv_file, label_file):
        self.csv_data = torch.Tensor(pd.read_csv(csv_file).to_numpy()[:, 1:]).to(device)
        self.label_data = torch.Tensor((pd.read_csv(label_file)).to_numpy()[:, 1]).view(-1).to(device)
        print(self.csv_data.size())
        print(self.label_data.size())
    def __len__(self):
        return len(self.csv_data)
    def __getitem__(self, idx):
        data = (self.csv_data[idx], self.label_data[idx])
        return data

dataset = RideHailingDataset("data.csv", "label.csv")
dataset_size = len(dataset)
print(dataset[0])

indices = list(range(dataset_size))
split = int(np.floor(validation_split * dataset_size))
if shuffle_dataset:
    np.random.seed(random_seed)
    np.random.shuffle(indices)
train_indices, val_indices = indices[split:], indices[:split]

train_sampler = sampler.SubsetRandomSampler(train_indices)
valid_sampler = sampler.SubsetRandomSampler(val_indices)

train_loader = DataLoader(dataset, batch_size=batch_size, sampler=train_sampler)
validation_loader = DataLoader(dataset, batch_size=batch_size, sampler=valid_sampler)