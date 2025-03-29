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

// 引入灯效
#include "light.h"

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

// 中断数据区域
volatile bool onoff = 0;
volatile int change = 0;
volatile bool onoff_pressed = false;
volatile bool change_pressed = false;
volatile unsigned long onoff_last_time = 0;
volatile unsigned long change_last_time = 0;
const unsigned long debounce_delay = 50000; // 消抖时间，单位为微秒（50 ms）

void onOffISR()
{
    onoff_last_time = micros();
    onoff_pressed = true;
}
void changeISR()
{
    change_last_time = micros();
    change_pressed = true;
}

void setup()
{
    // 串口初始化
    Serial.begin(9600);

    // LED 初始化
    // 初始化LED灯带<类型，引脚，颜色顺序>（名字，数量）
    LEDS.addLeds<LED_TYPE, DATA_IN, COLOR_ORDER>(leds, LED_NUM);
    // 设置所有LED的最大亮度
    LEDS.setBrightness(max_bright);

    // MAX4466初始化
    pinMode(DATA_OUT, INPUT);

    // 按键初始化
    pinMode(ON_OFF, INPUT_PULLDOWN);
    pinMode(Change, INPUT_PULLDOWN);

    // 设置外部中断
    attachInterrupt(digitalPinToInterrupt(ON_OFF), onOffISR, RISING);
    attachInterrupt(digitalPinToInterrupt(Change), changeISR, RISING);
}

void loop()
{
    // 按键消抖
    static unsigned long last_onoff_time = 0;
    static unsigned long last_change_time = 0;
    // 开关按键
    if (onoff_pressed)
    {
        unsigned long current_time = micros();
        if (current_time - onoff_last_time > debounce_delay)
        {
            onoff = !onoff;
            last_onoff_time = current_time;
        }
        onoff_pressed = false;
    }
    // 改变按键
    if (change_pressed)
    {
        unsigned long current_time = micros();
        if (current_time - change_last_time > debounce_delay)
        {
            if (change < lights-1)
            {
                change++;
            }
            else
            {
                change = 0;
            }
            last_change_time = current_time;
        }
        change_pressed = false;
    }
    Serial.print("onoff: ");
    Serial.println(onoff);
    Serial.print("change: ");
    Serial.println(light_names[change]);

    // 读取MAX4466的数据
    int dB = analogRead(DATA_OUT);
    Serial.println(dB);
    delay(500);
}