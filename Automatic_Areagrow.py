from pylab import *
import random

import cv2
import numpy as np
import math
import linecache
from sklearn.svm import SVC
from sklearn import tree


def Calculateparameter(Seeds, img):
    Seedspixelvalue = []
    for i in range(len(Seeds)):
        Seedspixelvalue.append(img[Seeds[i][0], Seeds[i][1]])
    sampleMean = np.mean(Seedspixelvalue)  # 随机点的均值和方差
    sampleStd = np.std(Seedspixelvalue)
    return sampleMean, sampleStd


def Automaticgrow(Seeds, Img):
    grayimg = cv2.cvtColor(Img, cv2.COLOR_BGR2GRAY)
    sampleMean, sampleStd = Calculateparameter(Seeds, grayimg)
    directs = [(-1, -1), (0, -1), (1, -1), (1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0)]
    visited = np.zeros(shape=(grayimg.shape), dtype=np.uint8)
    for i in range(len(Seeds)):
        # visited = np.zeros(shape=(grayimg.shape), dtype=np.uint8)
        Seed = []  # 每一个随机点进行区域增长
        centerx, centery = Seeds.pop(0)[:2]
        Seed.append([centerx, centery])
        count = 1
        alternativesseed = []
        alternativesseed.append([centerx, centery])
        seedmean1 = grayimg[centerx, centery]

        while len(Seed):
            seed = Seed.pop(0)
            x = seed[0]
            y = seed[1]
            visited[x][y] = 1
            for direct in directs:
                cur_x = x + direct[0]
                cur_y = y + direct[1]
                # 非法
                if cur_x < 0 or cur_y < 0 or cur_x >= visited.shape[0] or cur_y >= visited.shape[1]:
                    continue
                # 没有访问过且属于同一目标
                if (not visited[cur_x][cur_y]) and (sampleMean - 1 * sampleStd) <= grayimg[cur_x][cur_y] <= (
                        sampleMean + 1 * sampleStd):
                    visited[cur_x][cur_y] = 1
                    Seed.append((cur_x, cur_y))
                    count = count + 1
                    alternativesseed.append([cur_x, cur_y])

            seedmean2, seedstd2 = Calculateparameter(alternativesseed, grayimg)
            if (seedmean2 - seedmean1) < 0.001 and seedstd2 > 5:
                break
            else:
                seedmean1 = seedmean2

    return alternativesseed


def Piex(area, img):
    R = []
    G = []
    B = []
    for i in range(len(area)):
        x, y = area[i][0], area[i][1]
        R.append(img[x, y, 0])
        G.append(img[x, y, 1])
        B.append(img[x, y, 2])
        # gray.append(gray_img[x, y])
        mean_R = mean(R)
        mean_G = mean(G)
        mean_B = mean(B)

    return mean_R, mean_G, mean_B