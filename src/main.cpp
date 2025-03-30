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

/**
 * 引脚及其初始化
 *
 */
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
/***/

/**
 * 中断服务区
 *
 */
// 参数
volatile bool onoff = 0;
volatile int change = 0;
volatile bool onoff_pressed = false;
volatile bool change_pressed = false;
volatile unsigned long onoff_last_time = 0;
volatile unsigned long change_last_time = 0;
const unsigned long debounce_delay = 50000; // 消抖时间，单位为微秒（共50 ms）

// 中断函数
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
/***/

/**
 * 灯效服务区
 *
 */
// 灯效函数
// 彩虹
u_int8_t rainbow_start = 0; // 无需重置u_int8_t会自动初始化为0
void rainbow_ring(u_int8_t rainbow_start)
{
    fill_palette(leds, LED_NUM, rainbow_start, 8, RainbowColors_p, 255, LINEARBLEND);
    LEDS.show();
}

// 呼吸
// 初始化
u_int8_t breatheV = 32;
// 亮灭
bool breathe_on = true;
void breathe(u_int8_t breatheV)
{
    fill_solid(leds, 40, CHSV(174, 255, breatheV));
    LEDS.show();
}

// 流水
int flow_i = 0;
void flower(int flow_i)
{
    fill_solid(leds + flow_i, 3, CRGB::Red); // 亮灯
    if (flow_i >= 1 && flow_i < LED_NUM + 1) // 灭灯
    {
        fill_solid(leds + flow_i - 1, 1, CRGB::Black);
    }
    FastLED.show();
    delay(50); // 延时
}

// 切换
void function_light(int light_code)
{
    switch (light_code)
    {
    case 0: // 彩虹
        rainbow_start += 1;
        rainbow_ring(rainbow_start);
        break;
    case 1: // 呼吸
        // 加减判断
        if (breatheV == 32)
        {
            breathe_on = true;
        }
        if (breatheV == 255)
        {
            breathe_on = false;
        }
        // 渐变
        switch (breathe_on)
        {
        case true:
            breatheV += 1;
            break;
        case false:
            breatheV -= 1;
            break;
        }
        // 赋值给灯
        breathe(breatheV);
        break;
    case 2: // 流水
        flower(flow_i);
        flow_i += 1;
        if (flow_i >= LED_NUM + 3)
        {
            // 灯带末尾，清空灯带
            fill_solid(leds, 40, CRGB::Black); // 先清空灯带
            LEDS.show();                       // 显示清空
            flow_i = 0;
        }
        break;
    }
}
// 灯效个数
unsigned int lights = 3;
/***/

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
    /**
     * 按键消抖
     *
     */
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
            change<= lights - 1?change++:change=0;
            switch (change)
            {
            case 1: // 呼吸
                breatheV = 0;
                break;
            case 2:                                // 流水
                fill_solid(leds, 40, CRGB::Black); // 先清空灯带
                LEDS.show();                       // 显示清空
                flow_i = 0;
                break;
            }
            last_change_time = current_time;
        }
        change_pressed = false;
    }
    // Serial.print("onoff: ");
    // Serial.println(onoff);
    Serial.print("change: ");
    Serial.println(change);
    /***/

    /**
     * 灯效函数调用
     * 默认MAX4466关闭
     */
    if (!onoff)
    {
        function_light(change); // 默认灯效
    }
    else
    {
        // 读取MAX4466的数据
        int dB = analogRead(DATA_OUT);
        Serial.println(dB);
        delay(500);
    }
    /***/
}