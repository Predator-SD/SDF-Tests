# SDF-Tests

# 项目起源：</br>
### ZZYZ二楼机房空调不中用，冷到怀疑人生，写不进去题，又不甘于颓废，于是就开展了此学术（颓废）项目qwq</br>

# 技术资磁：</br>
### Predator_SD : SDF&绘图相关</br>
### __stdcall   : 多线程加速计算</br>
### Obsidian    : 降噪&放大技术</br>

# 关于效率：</br>
### 慢成狗，没有多线程优化几乎要上天QAQ</br>

# 效果一览：</br>
![](result.png)
</br>

# 封装解释：</br>
### config里存两个数 第一个是总机数x(由于某个我暂时懒得改的bug，必须是x=2^n)，第二个是本机id(1-x)</br>
### 一些参数在Entity.h里，可以自己调整：</br>
### N->积分精度</br>
### MAX_DEPTH->折射反射的最大次数</br>
### W,H->图片的长和宽</br>
### 场景Entity部署在Calc.h里。。。太麻烦了，我懒得解释了，大家看代码意会就行了。。。</br>

# 今后开发方向：</br>
### 1.更加优美的封装&绘图技术支持(achieved)</br>
### 2.分布式计算，榨干全机房电脑的性能(developing)</br>
### 3.光的色散效果(achieved)</br>
### 4.四叉树加速场景渲染(NULL)</br>

# 特别鸣谢：</br>
### __stdcall</br>
### Obsidian</br>
### 机房的监控服务器qwq</br>
