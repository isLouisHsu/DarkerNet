import cv2
import numpy
from parse import load_train_images, load_train_labels

def main():
    images = load_train_images('t10k-images-idx3-ubyte')    # 6000, 784
    labels = load_train_labels('t10k-labels-idx1-ubyte').astype('int')    # 6000

    f = open("list.txt", "w")
    cnt = [0 for i in range(10)]    # 0, 0, ..., 0

    for i in range(images.shape[0]):
        im = images[i].reshape([28, 28])
        lb = labels[i]
        name = "./images/{:d}_{:04d}.jpg".format(lb, cnt[lb])
        f.write(name.split('/')[-1] + '\n')
        cv2.imwrite(name, im)
        cnt[lb] += 1
    
    print(cnt)
    f.close()

if __name__ == "__main__":
    main()