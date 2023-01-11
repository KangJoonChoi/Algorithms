# -*- coding: utf-8 -*-
"""
Created on Tue Aug  2 01:14:09 2022

@author: 99kyu
"""

import torch

is_cuda = torch.cuda.is_available()
device = torch.device('cuda' if is_cuda else 'cpu')
batch_size = 256
epochs = 50

import torchvision.transforms as transforms
from torchvision.datasets import ImageFolder

transform_base = transforms.Compose([transforms.Resize((64, 64)),
                                     transforms.Grayscale(num_output_channels = 1),
                                     transforms.ToTensor()])

train_dataset = ImageFolder(root = r'C:/Users/nt110/OneDrive/Desktop/Crossroad/custom_cnn_model/dataset(64_gray)/splitted_gray_2/train',
                            transform = transform_base)
val_dataset = ImageFolder(root = r'C:/Users/nt110/OneDrive/Desktop/Crossroad/custom_cnn_model/dataset(64_gray)/splitted_gray_2/val',
                          transform = transform_base)

from torch.utils.data import DataLoader

train_loader = DataLoader(train_dataset,
                          batch_size = batch_size,
                          shuffle = True,
                          num_workers = 4)

val_loader = DataLoader(val_dataset,
                        batch_size = batch_size,
                        shuffle = True,
                        num_workers = 4)

import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim

class Basic_CNN_3(nn.Module):

    def __init__(self):
        super(Basic_CNN_3, self).__init__()
        self.keep_prob = 0.5

        self.layer1 = torch.nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=3, stride=1, padding=1),         #컬러 이미지이므로 channel수는 3이다(RGB). 3-channel 데이터를 convolution하여 32-channel로 만든다.
            nn.ReLU(),                                                    #비선형 활성화 함수
            nn.MaxPool2d(kernel_size=2, stride=2))                        #2x2마다 최대값만 남겨서 feature map의 사이즈를 1/4로 줄인다(길이는 1/2). 128x128 -> 64x64

        self.layer2 = torch.nn.Sequential(
            nn.Conv2d(32, 32, kernel_size=3, stride=1, padding=1),        #32-channel 데이터를 convolution하여 32-channel로 만든다.
            nn.BatchNorm2d(32),                                           #64x64의 feature map의 값을 정규화한다. (overfitting 방지)
            nn.ReLU(),
            nn.Dropout(0.25)
            )

        self.layer3 = torch.nn.Sequential(
            nn.Conv2d(32, 32, kernel_size=3, stride=1, padding=1),        #32-channel 데이터를 convolution하여 32-channel로 만든다.
            nn.BatchNorm2d(32))                                           #64x64의 feature map의 값을 정규화한다. (overfitting 방지)
        
        self.layer4 = torch.nn.Sequential(
            nn.ReLU(),
            nn.Dropout(0.25),
            nn.Conv2d(32, 64, kernel_size=3, stride=1, padding=1),        #32-channel 데이터를 convolution하여 64-channel로 만든다.)
            nn.AvgPool2d(kernel_size=2, stride=2))                        #2x2마다 평균값 하나를 남겨서 feature map의 사이즈를 1/4로 줄인다(길이는 1/2). 64x64 -> 32x32

        self.layer5 = torch.nn.Sequential(
            nn.Conv2d(64, 64, kernel_size=3, stride=1, padding=1),        #64-channel 데이터를 convolution하여 64-channel로 만든다.
            nn.BatchNorm2d(64),                                           #32x32의 feature map의 값을 정규화한다. (overfitting 방지)
            nn.ReLU(),
            nn.Dropout(0.25)
            )
        
        self.layer6 = torch.nn.Sequential(
            nn.Conv2d(64, 64, kernel_size=3, stride=1, padding=1),        #64-channel 데이터를 convolution하여 64-channel로 만든다.
            nn.BatchNorm2d(64))                                           #32x32의 feature map의 값을 정규화한다. (overfitting 방지)

        self.layer7 = torch.nn.Sequential(
            nn.ReLU(),
            nn.Dropout(0.25),
            nn.Conv2d(64, 64, kernel_size=3, stride=1, padding=1),        #64-channel 데이터를 convolution하여 128-channel로 만든다.
            nn.AvgPool2d(kernel_size=2, stride=2))                        #2x2마다 최대값만 남겨서 feature map의 사이즈를 1/4로 줄인다(길이는 1/2). 32x32 -> 16x16

        # L4 FC 4x4x128 inputs -> 625 outputs
        self.fc1 = nn.Linear(8 * 8 * 64, 512, bias=True)                #16x16의 feature map이 128channel이 있으므로 단일 유닛 값은 16 * 16 * 64개가 있다. 이것을 512개의 유닛과 연결시킨다.
        nn.init.xavier_uniform_(self.fc1.weight)

        self.layer8 = torch.nn.Sequential(
            self.fc1,
                nn.ReLU(),                                                #linear regression의 비선형 활성화 함수
                nn.Dropout(p=1 - self.keep_prob))                         #overfitting 방지(무작위로 노드 비활성화)
        
        self.fc2 = torch.nn.Linear(512, 5, bias=True)                           #Lable의 개수는 5개 이므로 최종 노드 개수는 5개로 설정한다.
        nn.init.xavier_uniform_(self.fc2.weight)
        
        
    def forward(self, x):
        init_out = self.layer1(x)                                               
        out = self.layer2(init_out)
        out = self.layer3(out)
        init_out = self.layer4(out + init_out)                                  #resnet의 구조를 구현하기 위해 3개의 convolution layer를 통과한 값(Batch_normalization과 Relu를 거침)과 통과 전의 값을 합한다.
        out = self.layer5(init_out)
        out = self.layer6(out)
        final_out = self.layer7(out + init_out)
        out = final_out.view(final_out.size(0), -1)                             #feature map들의 flatten data
        out = self.layer8(out)
        out = self.fc2(out)
        return out
    
    
    
    
model_base = Basic_CNN_3().to(device)
optimizer = optim.Adam(model_base.parameters(), lr = 0.0001)

def train(model, train_loader, optimizer):
    model.train()
    for batch_idx, (data, target) in enumerate(train_loader):
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)
        loss = F.cross_entropy(output, target)
        loss.backward()
        optimizer.step()
        
def evaluate(model, test_loader):
    model.eval()
    test_loss = 0
    correct = 0
        
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)
                
            test_loss += F.cross_entropy(output,
                                         target,
                                         reduction = 'sum').item()
            pred = output.max(1, keepdim = True)[1]
            correct += pred.eq(target.view_as(pred)).sum().item()
                
    test_loss /= len(test_loader.dataset)
    test_accuracy = 100. * correct / len(test_loader.dataset)
    return test_loss, test_accuracy
    
import time
import copy

def train_baseline(model, train_loader, val_loader, optimizer, epochs = 30):
    best_acc = 0.0
    best_model_wts = copy.deepcopy(model.state_dict())
    
    for epoch in range(1, epochs + 1):
        since = time.time()
        train(model, train_loader, optimizer)
        train_loss, train_acc = evaluate(model, train_loader)
        val_loss, val_acc = evaluate(model, val_loader)
        
        if val_acc > best_acc:
            best_acc = val_acc
            best_model_wts = copy.deepcopy(model.state_dict())
            
        time_elapsed = time.time() - since
        print('---------------------- epoch {} ----------------------'.format(epoch))
        
        print('train Loss: {:.4f}, Accuracy: {:.2f}%'.format(train_loss, train_acc))
        print('val Loss: {:.4f}, Accuracy: {:.2f}%'.format(val_loss, val_acc))
        print('Completed in {:.0f}m {:.0f}s'.format(time_elapsed // 60, time_elapsed % 60))
        
    model.load_state_dict(best_model_wts)
    return model

base = train_baseline(model_base,
                      train_loader,
                      val_loader,
                      optimizer,
                      epochs)

torch.save(base, 'F:/바탕화면/졸작/Code/Basic_CNN/processed_custom_ResNet_CNN_gray_64.pt')