# 不同类型的距离度量方法
距离是衡量两个向量相似度的方法，距离越远，则向量越不相似。现在业界中有若干距离度量方法，在这个项目中我选取了一部分进行解释，并在代码中实现。不同的距离度量方法，拥有不同的时间复杂度，不同的空间时间消耗，也拥有不同的度量效果。
## 欧式距离（欧几里得距离）（Euclidean Distance）
欧式距离是将两个向量每位的差值平方后加和，最后对和开根号得到的距离。在二维空间中表示两个点的直线距离。
![欧式距离](picture/euclidean_distance.png)

在项目中的实现为：
> include/distance/euclidean_distance.h

## 内积 （Inner Product）
如果两个向量定义如下
![内积1](picture/inner_product1.png)
那么它们的内积为
![内积2](picture/inner_product2.png)
在项目中的实现为：
> include/distance/inner_product.h

## todo


# 不同类型的索引
1、向量数据库的索引结构可以根据不同数量级的数据进行取舍。

# 索引选型

# 不同类型的距离度量