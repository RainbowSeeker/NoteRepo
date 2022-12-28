> n-frame : 北东地 		b-frame : 前右下

## INS

### 1.Sensor_PreProcess



1. IMU_PreProcess

![image-20221226195717989](./assets/image-20221226195717989.png)

![image-20221226195013313](./assets/image-20221226195013313.png)

当时间戳未更新时，对处理周期进行积分，在< timeout 时，数据有效。

![image-20221226195056508](./assets/image-20221226195056508.png)

其中的积分模块

![image-20221226193520702](./assets/image-20221226193520702.png)

`Ever_Vaild` 只要数据更新过，就为`True`。

![image-20221226195413642](./assets/image-20221226195413642.png)

![image-20221226195316852](./assets/image-20221226195316852.png)





2. MAG_PreProcess

![image-20221226200311129](./assets/image-20221226200311129.png)

> `M_OB` 表示 `DCM from body frame to earth frame`



通过GPS得到的当地经纬度数据，在全球地磁模型查找表（`WMM_LUT`）里查找出对应位置的磁场数据。

![image-20221226202107394](./assets/image-20221226202107394.png)

将弧度数据转换成 LUT 的 index， 这里除以 `resolution`因为 LUT 的分度值并未 1° （大约 10°）。

![image-20221226203357297](./assets/image-20221226203357297.png)

> 最终输出为当地的  磁场强度`magnitude_uT`， 磁偏角`declination_deg`， 磁倾角 `inclination_deg`

![image-20221226204916205](./assets/image-20221226204916205.png)



- WMM_Compensation

先将磁场数据除以当地磁场强度， 再将其从 b-frame 转到 e-frame （由于磁偏角的存在，经过DCM后 z-axis 还需转动 `-declination_rad`）。

![image-20221226210609891](./assets/image-20221226210609891.png)

只有在 GPS 数据有效的时候，才选用上一步处理完的数据，否则取磁力计规范化后的数据。

![image-20221226211458325](./assets/image-20221226211458325.png)

经过`DCM_BO`后，得到 b-frame 下的规范化的磁场数据。

![image-20221226211823672](./assets/image-20221226211823672.png)

- Mag_Quality_Estimate

![image-20221226220035706](./assets/image-20221226220035706.png)

对磁密度以及磁倾角的质量进行测算， 输出经过二阶`LPF`。

![image-20221226220143720](./assets/image-20221226220143720.png)



3. GPS_PreProcess

![image-20221226223159670](./assets/image-20221226223159670.png)



- quality

求取三项指标的质量。

![image-20221226224552064](./assets/image-20221226224552064.png)

- gps_status

判断 hAcc， fixtype 等是否符合要求，其中Ready模块作用为 当valid 累计达到 1000次true时，GPS 变为就绪状态。

![image-20221226224048085](./assets/image-20221226224048085.png)

![image-20221226224355190](./assets/image-20221226224355190.png)



- WGS84_Model

![image-20221226225928751](./assets/image-20221226225928751.png)

![image-20221226225919880](./assets/image-20221226225919880.png)

使用纬度求取地球扁平率。



4. Baro_PreProcess

![image-20221226230847900](./assets/image-20221226230847900.png)

- Pressure_Height

通过压力计算**绝对高度**。

![image-20221226230820082](./assets/image-20221226230820082.png)

- Relative_Height

![image-20221226232015955](./assets/image-20221226232015955.png)

其中`Sampling` 将启动前200次采样，采样完成则初始化成功。

![image-20221226232114290](./assets/image-20221226232114290.png)

其中`Reference_Height` 将保存初始化后的高度均值。

![image-20221226232315518](./assets/image-20221226232315518.png)



### 2.Data_Fusion



数据融合包括两部分： 旋转滤波 + 转义滤波
![image-20221227120827023](./assets/image-20221227120827023.png)



#### 1.RF_Data_PreProcess

![image-20221227153023815](./assets/image-20221227153023815.png)

##### Standstill 模块

![image-20221227153217503](./assets/image-20221227153217503.png)

取 x, y 轴角速度，经过 fc = 0.25 Hz 的 HPF 

![image-20221227154020936](./assets/image-20221227154020936.png)

再经过两次 LPF 后对信号进行判断。

##### Attitude_Reset

![image-20221227160501545](./assets/image-20221227160501545.png)

`att_reset` 信号在`sample_valid` 信号上升沿（第一次变为有效）或`GPS valid` 时有效。

`sample_valid` 表示当 `valid` 次数到达50次时， 输出信号为 true。

![image-20221227160821753](./assets/image-20221227160821753.png)

![image-20221227160902847](./assets/image-20221227160902847.png)

![image-20221227161414752](./assets/image-20221227161414752.png)

##### Initial_Attitude

通过加速度计 和 磁力计 求取初始姿态。

![image-20221227165202760](./assets/image-20221227165202760.png)

加速度（g）反向与磁场向量叉积，

![image-20221227165620692](./assets/image-20221227165620692.png)

![image-20221227130308086](./assets/image-20221227130308086.png)

> 为什么求解后的DCM 是from n-frame to b-frame ?
>
> DCM 的第 1 列就是 n-frame的 x 轴上的的单位向量 在 b-frame 下的坐标。
>
> DCM 的第 1 行就是 b-frame的 x 轴上的的单位向量 在 n-frame 下的坐标。
>
> **实际上就是**：如果 DCM 的参照系为b-frame， 那么最终坐标就应该转到 b-frame。

![image-20221227180557740](./assets/image-20221227180557740.png)



#### 2.AHRS

正式计算姿态信息。

##### 1.Correct

包含姿态校正和陀螺零偏校正。

![image-20221227184553179](./assets/image-20221227184553179.png)

###### Attitude_Correction

包含姿态以及磁力的校正

![image-20221227190651492](./assets/image-20221227190651492.png)

1. Attitude_Correct

姿态校正包括加速度的估算与校正。

![image-20221227190853916](./assets/image-20221227190853916.png)

- Acc_Estimator

加速度估算包括获取 GPS 测量的载体速度， 以及利用姿态信息辅助估算速度。

![image-20221227191019593](./assets/image-20221227191019593.png)

- Measure_Acc

包括检查有效性，总线读取GPS速度信息。最终输出 n-frame 下的gps 加速度。

![image-20221227191216987](./assets/image-20221227191216987.png)

- GPS_Meas_Acc

[TODO] 原理？如何由速度得到平均加速度。

![image-20221227193815680](./assets/image-20221227193815680.png)

- Estimate_Acc

将加速度转到 n-frame 下，使用二阶 LPF 滤波，并在gps数据有效时将其延迟200ms （GPS数据更新延迟）。

![image-20221227194949566](./assets/image-20221227194949566.png)

- Heading_Correct

去除 估计和平均加速度的z 轴数据 （获取航向）并取叉积，得到航向校正矩阵 （绕z 轴的旋转）。

![image-20221227210207128](./assets/image-20221227210207128.png)

- Verticle_Correct

[TODO] 原理？

![image-20221227212951698](./assets/image-20221227212951698.png)





##### 2.Update

- Attitude_Update

使用修正后的角速度（互补滤波值）进行姿态更新。

![image-20221228104700771](./assets/image-20221228104700771.png)

更新算法为四元数微分方程的一阶**龙格库塔**离散化。
$$
\frac{\mathrm{d}Q}{\mathrm{d}t} = \frac{1}{2}\cdot\begin{bmatrix}
0 & -w_x & -w_y & -w_z \\
w_x & 0 & w_z & -w_y \\
w_y & -w_z & 0 & w_x \\
w_z & w_y & -w_x & 0
\end{bmatrix}
\begin{bmatrix}
q_0 \\ q_1 \\ q_2 \\ q_3
\end{bmatrix}\\
 using\quad RK1:y_{n+1}=y_n+h\cdot\dot y \\
 Q(t+\Delta t)=Q(t)+\Delta t \frac{\mathrm{d}Q}{\mathrm{d}t}\\
 also\quad\begin{bmatrix}q_0 \\ q_1 \\ q_2 \\ q_3\end{bmatrix}_{t+\Delta t} = \begin{bmatrix}q_0 \\ q_1 \\ q_2 \\ q_3\end{bmatrix}_{t} + \frac{\Delta t}{2} \begin{bmatrix}
0 & -w_x & -w_y & -w_z \\
w_x & 0 & w_z & -w_y \\
w_y & -w_z & 0 & w_x \\
w_z & w_y & -w_x & 0
\end{bmatrix}
\begin{bmatrix}
q_0 \\ q_1 \\ q_2 \\ q_3
\end{bmatrix}_{t}\\
$$
![image-20221228105918036](./assets/image-20221228105918036.png)
