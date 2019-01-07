clc;
clear all;
close all;
%randsrc是根据给定的数字按给定概率产生一个随机符号矩阵
a=randsrc(1,20,[0:1;0.4,0.6]);      %调制信号
l=linspace(0,2*pi,50);         %产生0~2*pi之间50个抽样值
f=sin(2*l);         %正弦波信号
t=linspace(0,10,1000);
out=1:1000;
b=1:1000;
w=1:1000;
for i=1:20          %产生与时间间隔相同数组元素
    for j=1:50 
        b(j+50*(i-1))=a(i); 
        w(j+50*(i-1))=f(j); 
    end 
end 
%产生2ASK信号，out信号为2ASK信号
for i=1:20 
    if a(i)==0 
      for j=1:50 
          out(j+50*(i-1))=0; 
      end 
    else 
      for j=1:50 
          out(j+50*(i-1))=5*f(j);   %信号放大
      end 
    end 
end 
subplot(421),plot(t,b),axis([0 10 -1 1.5]), xlabel('t'),ylabel('幅度'),title('调制信号');grid on; 
subplot(422),plot(t,w),axis([0 10 -1.5 1.5]), xlabel('t'),ylabel('幅度'),title('载波'); grid on;
subplot(423),plot(t,out),axis([0 10 -7.5 7.5]),xlabel('t'),ylabel('幅度'),title(' ASK波形');grid on;

noise=awgn(out,5,'measured') ;      %awgn用于产生高斯白噪声
subplot(424); plot(t,noise);axis([0 10 -5.5 5.5]);xlabel('t');ylabel('幅度');title('噪音+信号'); grid on;

%带通滤波器实现
Fs=100;%
t=(1:1000)/Fs;%
[b,a]=ellip(4,0.1,40,[2,6]*2/Fs);%8阶椭圆带通滤波器，0.1db通带波纹，40db阻带衰减，100Hz采样频率
sf=filter(b,a,noise);%1
%figure(2)
subplot(425); 
plot(t,sf);
xlabel('t'); ylabel('幅度');
title('通过BPF后的波形'); 
axis([0 10 -5.5 5.5]);
grid on;

f=[f f f f f f f f f f f f f f f f f f f f];
s=sf.*f;
subplot(426); 
plot(t,s);%
xlabel('t'); ylabel('幅度');
title('通过相乘器后波形'); 
axis([0 10 -6 6]);
grid on;

Fs=100;
t=(1:1000)/Fs;
[b,a]=ellip(4,0.1,40,[2]*2/Fs);%8阶椭圆低通滤波器，0.1db通带波纹，40db阻带衰减，100Hz采样频率
sf=filter(b,a,s);
subplot(428); 
plot(t,sf);
xlabel('t'); ylabel('幅度');
title('通过LPF后的波形'); 
axis([0 10 -3 3]);
grid on;

%抽样判决
b=1.2;
for i=1:20 
  for j=1:50 
     if sf(j+50*(i-1))>b 
        sf(j+50*(i-1))=1; 
     else 
        sf(j+50*(i-1))=0; 
     end 
  end 
end
subplot(427); 
plot(t,sf);
xlabel('t'); ylabel('幅度');
title('抽样判决后波形'); 
axis([0 10 -1 1.2]);
grid on;