# FFT_STM32
一个stm32上使用LCD显示fft结果的例程
使用三通道adc模拟输入，每个adc输入的结果通过一个3x1模板进行滤波；
滤波后结果作为fft函数的输入（fft函数使用dsp库自带）；
输出处理后进行归一化后在LCD屏幕上进行显示。
