# TC4-WB
#### TC4-WB is a esp32 base thermo module for Artisan 

1) it can get **two channels**  temperture data by MX6675 For BT channel, data updates every 750ms and ET channel data updates every 3s .

2) The temperture datas can transmit with Artisan by wifi-websocket and/or bluetooth-TC4:

**base on the version you chioced .**

**full version :**  Both wifi-websocket and bluetooth ,so you can monitor the data on PC and cellphone both 

**wifi version:**  Only wifi-websocket,so you can monitor data on PC on tablet by wifi.

**bluetooth version** : Only bluetooth ,you can monitor data by bluetooth .
      
3) ALL version can use OTA to update firmware by wifi .

    
4) Thermo compensate funciton also include.

### 中文说明
#### TC4-wb 是个基于esp32 芯片的温度传感器模块，用于Artisan
       
1) 模块有**两路** MAX6675 温度采集芯片，分别输出到BT（豆温）和ET（炉温）豆温采集时间是750ms，炉温采集时间是3s

2) 模块可以通过蓝牙 和 wifi-websocket方式传输 TC4 协议的数据到电脑（win linux macos / wifi ）、安卓手机、平板（蓝牙）

**根据需求选择对应的固件**：

**full version**:全功能版本，包括了wifi和蓝牙方式的数据传输方式

**wifi version**:只有Wi-Fi/websocket传输方式。

**bluetooth version**：只有蓝牙数据输出方式

3) 所有版本都可以通过OTA方式在本地wifi环境下升级固件

4) 包含了温度补偿自定义的方式


### 公测版说明
参加公测版的小伙伴请阅读下面说明，如果只是使用的话就可以忽略：
#### 规则简单说明：
在小黄鱼上买了公测版硬件之后，

1) 对**代码**和**硬件**提出功能修改意见并验证通过的。一项返现RMB50元。

2) 对**代码**中存在的bug 修复并通过验证的。一项返现RMB30元。

3) 帮忙**代码** 整理、注释的小伙伴（3个名额）。一位返现10元。

4) 返现的可以叠加使用，直到抵扣完模块的价款（最多288元，因为卖的就是这个价格，能整合5项功能进来这个项目，我白送一个硬件也值得了）

5) 代码设计文件通过G站和gitee提交都可以。我会定期同步的，gitee是备份站。


比如:小黄鱼 [传送门](https://m.tb.cn/h.UNjM5cI?tk=xDD2dhO9j6p) (请留意：其他渠道的我就帮不上忙了) 上买到手之后，想添加个ROR 显示功能什么的。报功能清单，然后写好代码，提交，我会测试验证的，通过之后我就并入主线发布新版本，之后我会联系你返现50元的。

#### 功能需求清单

1) 模块使用过程中，插入电源会有机会OLED花屏（硬件BUG，50元）

2) 模块硬件布线检查、提高稳定性（硬件功能提升，50元）


