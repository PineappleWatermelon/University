clc;
clear all;
close all;
a=randsrc(1,20,[0:1;0.4,0.6]);
l=linspace(0,2*pi,50);
f=sin(2*l);
t=linspace(0,10,1000);
out=1:1000;
b=1:1000;
w=1:1000;
for i=1:20  
    for j=1:50 
        b(j+50*(i-1))=a(i); 
        w(j+50*(i-1))=f(j); 
    end 
end 
for i=1:20 
    if a(i)==0 
      for j=1:50 
          out(j+50*(i-1))=0; 
      end 
    else 
      for j=1:50 
          out(j+50*(i-1))=5*f(j); 
      end 
    end 
end 
subplot(421),plot(t,b),axis([0 10 -1 1.5]), xlabel('t'),ylabel('幅度'),title('调制信号');grid on; 
subplot(422),plot(t,w),axis([0 10 -1.5 1.5]), xlabel('t'),ylabel('幅度'),title('载波'); grid on;
subplot(423),plot(t,out),axis([0 10 -7.5 7.5]),xlabel('t'),ylabel('幅度'),title(' ASK波形');grid on;

noise=awgn(out,5,'measured') ;
subplot(424); plot(t,noise);axis([0 10 -5.5 5.5]);xlabel('t');ylabel('幅度');title('噪音+信号'); grid on;

Fs=100;%
t=(1:1000)/Fs;%
[b,a]=ellip(4,0.1,40,[2,6]*2/Fs);%
sf=filter(b,a,noise);%
%figure(2)
subplot(425); 
plot(t,sf);%
xlabel('t'); ylabel('幅度');
title('通过BPF后的波形'); 
axis([0 10 -5.5 5.5]);
grid on;

f=[f f f f f f f f f f f f f f f f f f f f];%
s=sf.*f;
subplot(426); 
plot(t,s);%
xlabel('t'); ylabel('幅度');
title('通过相乘器后波形'); 
axis([0 10 -6 6]);
grid on;

Fs=100;
t=(1:1000)/Fs;
[b,a]=ellip(4,0.1,40,[2]*2/Fs);
sf=filter(b,a,s);
subplot(428); 
plot(t,sf);
xlabel('t'); ylabel('幅度');
title('通过LPF后的波形'); 
axis([0 10 -3 3]);
grid on;

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