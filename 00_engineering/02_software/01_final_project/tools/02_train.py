import sys
import re
import math
import time
import numpy
import sqlite3
import os
import cv2
import torch
import torch.cuda
import torch.optim
import torch.jit
import torch.utils.data

class Dataset(torch.utils.data.Dataset):

    def __init__(self, root_directory, db, name):

        torch.utils.data.Dataset.__init__(self)

        self.root_directory = root_directory

        c = db.cursor()
        c.execute('SELECT image, steering FROM samples WHERE dataset_id IN (SELECT id FROM datasets WHERE name=?)', (name,))
        self.table = c.fetchall()
        c.close()

        print("Dataset {}: {} samples".format(name, len(self.table)))

    def __getitem__(self, index):

        # load image.
        image_path = os.path.join(self.root_directory, self.table[index][0])
        im = cv2.imread(image_path)

        # convert to RGB.
        im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)

        # convert to floating point.
        im = numpy.asarray(im, dtype=numpy.float32)

        # normalize.
        im /= 255.0

        mean = [0.485, 0.456, 0.406]
        sigma = [0.229, 0.224, 0.225]

        im[:,:,0] = (im[:,:,0] - mean[0]) / sigma[0]
        im[:,:,1] = (im[:,:,1] - mean[1]) / sigma[1]
        im[:,:,2] = (im[:,:,2] - mean[2]) / sigma[2]

        # make pytorch tensor.
        im = torch.tensor(im, dtype=torch.float32)
        im = im.permute([2, 0, 1]);

        steering = self.table[index][1]
        steering = torch.tensor([steering], dtype=torch.float32)

        return im, steering

    def __len__(self):
        return len(self.table)

class Model(torch.nn.Module):

    def __init__(self):

        torch.nn.Module.__init__(self)

        self.submodel = torch.hub.load('pytorch/vision:v0.5.0', 'resnet18', pretrained=True)
        for name, param in self.submodel.named_parameters():
            if "bn" not in name:
                param.requires_grad = False

        # TODO: consider other models!
        M = 800
        N = 1

        self.submodel.fc = torch.nn.Sequential(
            torch.nn.Linear(self.submodel.fc.in_features, M),
            torch.nn.ReLU(),
            torch.nn.Dropout(),
            torch.nn.Linear(M, N))

    def forward(self, x):
        return self.submodel(x)

def get_or_create_model():

    A = [ re.match("^checkpoint_([0-9]*).pth$", f) for f in os.listdir(".") ]
    B = [ (int(m.group(1)), m.group(0)) for m in A if m ]

    if B:
        B.sort(reverse=True)
        filename = B[0][1]
        print("Loading checkpoint {}".format(filename))
        model = torch.load(filename)
        model.eval()
    else:
        model = Model()

    #########""
    #m = torch.jit.script(model)
    #m.save("model.zip")
    #exit(0)
    #########""


    return model

if __name__ == '__main__':

    if len(sys.argv) != 2:
        print("Bad command line!")
        exit(1)

    if not torch.cuda.is_available:
        print("CUDA unavailable!")

    root_directory = sys.argv[1]

    db = sqlite3.connect(os.path.join(root_directory, 'db.sqlite'))
    train_loader = torch.utils.data.DataLoader(Dataset(root_directory, db, 'train'), batch_size=128)
    test_loader = torch.utils.data.DataLoader(Dataset(root_directory, db, 'test'), batch_size=128)
    db.close()

    device = 'cuda'

    model = get_or_create_model()
    model = model.to(device)

    learning_rate = 0.0001
    opt = torch.optim.Adam(model.parameters(), lr = learning_rate)

    loss_fn = torch.nn.MSELoss()

    for epoch in range(10):

        # training

        count = 0
        list_MSE = list()

        for image, ref_steering in train_loader:

            image = image.to(device)
            ref_steering = ref_steering.to(device)

            opt.zero_grad()
            steering = model(image)
            loss = loss_fn(steering, ref_steering)
            loss.backward()
            opt.step()

            print("[{}] MSE = {}".format(count, loss.item()))
            list_MSE.append(loss.item())
            count += 1

        print("Mean MSE on train set: {}", numpy.mean(list_MSE))

        # checkpoint

        number = math.floor(time.time())
        filename = "checkpoint_{}.pth".format(number)
        torch.save(model, filename)
        print("Checkpoint saved to {}".format(filename))

        # testing

        count = 0
        list_MSE = list()

        with torch.no_grad():
            for image, ref_steering in test_loader:

                image = image.to(device)
                ref_steering = ref_steering.to(device)

                steering = model(image)
                loss = loss_fn(steering, ref_steering)
                print("[{}] MSE = {}".format(count, loss.item()))
                list_MSE.append(loss.item())
                count += 1

        print("Mean MSE on test set: {}", numpy.mean(list_MSE))

