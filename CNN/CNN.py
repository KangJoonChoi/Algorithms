import torch
import torchvision.datasets
import torchvision.transforms as transforms
import torch.nn.init

device = 'cuda' #GPU 사용

torch.manual_seed(777) #랜덤 시드 고정

if device == 'cuda':
  torch.cuda.manual_seed_all(777) 

learning_rate = 0.001 #학습률:0.001
training_epochs = 10  #학습회수: 10번
batch_size = 15       #배치사이즈 : 15
######### CIFAR10 dataset 로드
transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize((0.5,0.5,0.5),(0.5,0.5,0.5))])

trainset = torchvision.datasets.CIFAR10(root = './data', train = True, download = True, transform = transform)
trainloader = torch.utils.data.DataLoader(trainset, batch_size = batch_size, shuffle = True, num_workers = 2)

testset = torchvision.datasets.CIFAR10(root = './data', train = False, download= True, transform = transform)
testloader = torch.utils.data.DataLoader(testset, batch_size = batch_size, shuffle = False, num_workers = 2)

classes = ('plane', 'car', 'bird','cat','deer','dog','frog','horse','ship','truck')
##########
class CNN(torch.nn.Module): ###CNN 구조 설정
  def __init__(self):
    super(CNN, self).__init__()
    self.keep_prob = 0.5 #오버피팅 방지 위한 Fully Connected Layer Dropout 시행을 위한 변수
    self.layer1 = torch.nn.Sequential( #첫번쨰 layer
        torch.nn.Conv2d(3,32,kernel_size = 4, stride = 1, padding = 1), #3Mnist와 같이 1채널이 아닌 3개의 채널을 가지고 있어 input == 3
        torch.nn.ReLU(), #커널 사이즈도 4로 변화를 줘보았다, Activation 을 위한 Relu
        torch.nn.MaxPool2d(kernel_size = 2, stride = 2) #풀링 2X2 커널 사용 스트라이드 : 2
    )
    self.layer2 = torch.nn.Sequential( 
        torch.nn.Conv2d(32,64,kernel_size = 4, stride = 1, padding = 1), #인풋 다이멘션 32로 바뀜
        torch.nn.ReLU(), #나머지는 동일
        torch.nn.MaxPool2d(kernel_size = 2, stride = 2)
    )
    self.layer3 = torch.nn.Sequential( 
        torch.nn.Conv2d(64,128,kernel_size = 4, stride = 1, padding = 1), #인풋 다이멘션 64로 바뀌고
        torch.nn.ReLU(), #나머지 동일
        torch.nn.MaxPool2d(kernel_size = 2, stride = 2)
    )
    self.fc1 = torch.nn.Linear(1152, 625, bias = True) #3X3X128 input을 Flatten 시켜 625 output linear 모델의 결과값 출력 
    torch.nn.init.xavier_uniform_(self.fc1.weight) #Weight Initialization을 위해 사용
    self.layer4 = torch.nn.Sequential( #Dropout 사용하여 Overfitting 완화
        self.fc1, torch.nn.ReLU(),
        torch.nn.Dropout(p = 1 - self.keep_prob))
    
    self.fc2 = torch.nn.Linear(625,10,bias = True) #625input을 10개의 output(10개의 클래스가 있으니)으로 출력
    torch.nn.init.xavier_uniform_(self.fc2.weight)

  def forward(self,x): #데이터를 순차적으로 넘겨주는 역할
    out = self.layer1(x) #X를 받아 layer통과 후 Out으로 저장
    out = self.layer2(out)
    out = self.layer3(out)
    out = out.view(out.size(0), -1)
    out = self.layer4(out)
    out = self.fc2(out)
    return out #최종 리턴

model = CNN().to(device) #모델을 미리 정의해뒀던 CNN으로 설정
criterion = torch.nn.CrossEntropyLoss().to(device) #Cost에 Softmax가 포함돼있음
optimizer = torch.optim.Adam(model.parameters(), lr = learning_rate) #옵티마이저 설정
total_batch = len(trainloader) 
print('총 배치의 수 : {}'.format(total_batch))

for epoch in range(training_epochs): #학습을 위한 반복
  avg_cost = 0 #초기화

  for X,Y in trainloader: #X,Y값을 trainloader로부터 받아서
    X = X.to(device)
    Y = Y.to(device)

    optimizer.zero_grad() #역전파를 위해 전epoch변수가 현 epoch update에 피해를 주지 않게 하기 위해초기화
    hypothesis = model(X) #X값 예측
    cost = criterion(hypothesis, Y) #label과 예측값과의 차이 비교
    cost.backward() #역전파
    optimizer.step() #매개변수 조정

    avg_cost += cost/total_batch #평균 코스트 계산
  
  print('[Epoch: {:>4}] cost = {:>.9}'.format(epoch + 1, avg_cost))

  with torch.no_grad(): # 테스트 시작
    for X_test, Y_test in testloader: 
      X_test = X_test.to(device)
      Y_test = Y_test.to(device)
      break #값을 하나씩 불러오고 싶어서 한번만 X_test,Y_test 값을 받고 break시켰음

    prediction = model(X_test) #test셋에 대한 예측
    correct_prediction = torch.argmax(prediction, 1) == Y_test #옳바른 예측이면 1, 아니면 0
    accuracy = correct_prediction.float().mean() #평균값 구한다
    print('Accuracy:', accuracy.item()) #정확도 출력
