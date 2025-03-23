/***
 * 我的灯带布局
 * 9 <8 <7 <6 <5 <4 <3 <2 <1 <0
 * 10>11>12>13>14>15>16>17>18>19
 * 29<28<27<26<25<24<23<22<21<20
 * 30>31>32>33>34>35>36>37>38>39
 * 
 * LED数据pin
 * @DATA_IN 12
 * 
 * MAX4466数据pin
 * @DATA_OUT 34
 * 
 * 按键pin
 * @Change 32
 * @ON_OFF 33
 */
#include <Arduino.h>
#include <fastLED.h>

// LED参数
// LED 的配置参数
#define DATA_IN 12
#define LED_NUM 40
// LED 的类型和颜色顺序
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
// 设置最大亮度
uint8_t max_bright = 32;
// LED灯带对象
CRGB leds[LED_NUM];

// MAX4466参数
#define DATA_OUT 34

// 按键参数
#define ON_OFF 33
#define Change 32

// function区域

void setup()
{
    // 初始化LED灯带<类型，引脚，颜色顺序>（名字，数量）
    LEDS.addLeds<LED_TYPE, DATA_IN, COLOR_ORDER>(leds, LED_NUM);
    // 设置所有LED的最大亮度
    LEDS.setBrightness(max_bright);
}

void loop()
{
}