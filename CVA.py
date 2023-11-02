import cv2
import numpy as np
import imageio
from osgeo import gdal
import time
from Automatic_Areagrow import Automaticgrow, Piex
from Areagrow import Areagrow_parameter, Calculate_mean


def stad_img(img, channel_first=True, get_para=False):
    """
    normalization image
    :param channel_first:
    :param img: (C, H, W)
    :return:
        norm_img: (C, H, W)
    """
    if channel_first:
        channel, img_height, img_width = img.shape
        img = np.reshape(img, (channel, img_height * img_width))  # (channel, height * width)
        mean = np.mean(img, axis=1, keepdims=True)  # (channel, 1)
        center = img - mean  # (channel, height * width)
        var = np.sum(np.power(center, 2), axis=1, keepdims=True) / (img_height * img_width)  # (channel, 1)
        std = np.sqrt(var)  # (channel, 1)
        nm_img = center / std  # (channel, height * width)
        nm_img = np.reshape(nm_img, (channel, img_height, img_width))
    else:
        img_height, img_width, channel = img.shape
        img = np.reshape(img, (img_height * img_width, channel))  # (height * width, channel)
        mean = np.mean(img, axis=0, keepdims=True)  # (1, channel)
        center = img - mean  # (height * width, channel)
        var = np.sum(np.power(center, 2), axis=0, keepdims=True) / (img_height * img_width)  # (1, channel)
        std = np.sqrt(var)  # (channel, 1)
        nm_img = center / std  # (channel, height * width)
        nm_img = np.reshape(nm_img, (img_height, img_width, channel))
    print('mean is ', mean)
    print('std is ', std)
    if get_para:
        return nm_img, mean, std
    else:
        return nm_img


def otsu(data, num=400, get_bcm=False):
    """
    generate binary change map based on otsu
    :param data: cluster data
    :param num: intensity number
    :param get_bcm: bool, get bcm or not
    :return:
        binary change map
        selected threshold
    """
    max_value = np.max(data)
    min_value = np.min(data)

    total_num = data.shape[1]
    step_value = (max_value - min_value) / num
    value = min_value + step_value
    best_threshold = min_value
    best_inter_class_var = 0
    while value <= max_value:
        data_1 = data[data <= value]
        data_2 = data[data > value]
        if data_1.shape[0] == 0 or data_2.shape[0] == 0:
            value += step_value
            continue
        w1 = data_1.shape[0] / total_num
        w2 = data_2.shape[0] / total_num

        mean_1 = data_1.mean()
        mean_2 = data_2.mean()

        inter_class_var = w1 * w2 * np.power((mean_1 - mean_2), 2)
        if best_inter_class_var < inter_class_var:
            best_inter_class_var = inter_class_var
            best_threshold = value
        value += step_value
    if get_bcm:
        bwp = np.zeros(data.shape)
        bwp[data <= best_threshold] = 0
        bwp[data > best_threshold] = 255
        print('otsu is done')
        return bwp, best_threshold
    else:
        print('otsu is done')
        return best_threshold


def CVA(img_X, img_Y, stad=False):
    # CVA has not affinity transformation consistency, so it is necessary to normalize multi-temporal images to
    # eliminate the radiometric inconsistency between them
    if stad:
        img_X = stad_img(img_X)
        img_Y = stad_img(img_Y)

    img_X = list(eval(str(img_X).replace("[", '').replace("]", '')))
    img_Y = list(eval(str(img_Y).replace("[", '').replace("]", '')))
    img_X = np.array(img_X)
    img_Y = np.array(img_Y)
    # print(img_X)
    # print(img_Y)
    # print(img_X.shape)
    # print(img_Y.shape)
    img_diff = img_X - img_Y

    L2_norm = np.sqrt(np.sum(np.square(img_diff), axis=0))
    return L2_norm


def main():
    # T1 = gdal.Open("..\\data\\Wetland\\PreImg_2006.img")
    # T2 = gdal.Open("..\\data\\Wetland\\PostImg_2007.img")
    # #
    # img_width = T1.RasterXSize  # image width
    # img_height = T2.RasterYSize  # image height
    #
    # img_X = T1.ReadAsArray(0, 0, img_width, img_height)z
    # img_Y = T2.ReadAsArray(0, 0, img_width, img_height)
    tic = time.time()
    img_X = cv2.imread("C:\\data\\Hermiston3\\result\\Filter_image-3.bmp")
    img_Y = cv2.imread("C:\\data\\Hermiston3\\result1\\Filter_image-3.bmp")
    img_height, img_width, channel = img_X.shape
    parameter = [[5, 50], [10, 50], [15, 50], [20, 50], [25, 50], [5, 60], [10, 60], [15, 60], [20, 60], [25, 60],
                 [5, 70], [10, 70], [15, 70], [20, 70], [25, 70], [5, 80],
                 [10, 80], [15, 80], [20, 80], [25, 80], [5, 90], [10, 90], [15, 90], [20, 90], [25, 90], [5, 100],
                 [15, 100], [20, 100], [25, 100]]
    index = 0
    result_img = np.zeros((img_X.shape[0], img_X.shape[1]))
    for i in range(img_height):
        for j in range(img_width):
            index += 1
            new_T1 = []
            new_T2 = []
            if index % 10000 == 0 :
                print("第%d个像素点作为种子点" % index)
            for k in range(len(parameter)):
                area1 = Areagrow_parameter([[i, j]], img_X, parameter[k][0], parameter[k][1])
                area2 = Areagrow_parameter([[i, j]], img_Y, parameter[k][0], parameter[k][1])
                piexR, piexG, piexB = Calculate_mean(area1, img_X)
                piexR1, piexG1, piexB1 = Calculate_mean(area2, img_Y)
                new_T1.append([piexR, piexG, piexB])
                new_T2.append([piexR1, piexG1, piexB1])
                result_img[i][j] = CVA(new_T1, new_T2)
    # print(img_X.shape)

    # L2_norm = CVA(img_X, img_Y)
    # imageio.imwrite('CVA_Wetland2.bmp', L2_norm)
    for k in range(len(parameter)):
        cv2.imwrite('D:\\Filter_result\\Hermiston\\CMI_3\\CVA_Hermiston_3_%d_%d.bmp' %
                    (parameter[k][0], parameter[k][1]), result_img)
        # print(L2_norm.shape)

        bcm = np.ones((img_height, img_width))
        thre = otsu(result_img.reshape(1, -1))
        bcm[result_img > thre] = 255
        bcm[result_img < thre] = 0
        bcm = np.reshape(bcm, (img_height, img_width))
        print(bcm.shape)
        cv2.imwrite('D:\\Filter_result\\Hermiston\\2-CD_3\\CVA_Hermiston_3_%d_%d.bmp' %
                    (parameter[k][0], parameter[k][1]), bcm)
    toc = time.time()
    print(toc - tic)


if __name__ == '__main__':
    main()
