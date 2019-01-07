clc;
clear all;
close all;
%randsrc�Ǹ��ݸ��������ְ��������ʲ���һ��������ž���
a=randsrc(1,20,[0:1;0.4,0.6]);      %�����ź�
l=linspace(0,2*pi,50);         %����0~2*pi֮��50������ֵ
f=sin(2*l);         %���Ҳ��ź�
t=linspace(0,10,1000);
out=1:1000;
b=1:1000;
w=1:1000;
for i=1:20          %������ʱ������ͬ����Ԫ��
    for j=1:50 
        b(j+50*(i-1))=a(i); 
        w(j+50*(i-1))=f(j); 
    end 
end 
%����2ASK�źţ�out�ź�Ϊ2ASK�ź�
for i=1:20 
    if a(i)==0 
      for j=1:50 
          out(j+50*(i-1))=0; 
      end 
    else 
      for j=1:50 
          out(j+50*(i-1))=5*f(j);   %�źŷŴ�
      end 
    end 
end 
subplot(421),plot(t,b),axis([0 10 -1 1.5]), xlabel('t'),ylabel('����'),title('�����ź�');grid on; 
subplot(422),plot(t,w),axis([0 10 -1.5 1.5]), xlabel('t'),ylabel('����'),title('�ز�'); grid on;
subplot(423),plot(t,out),axis([0 10 -7.5 7.5]),xlabel('t'),ylabel('����'),title(' ASK����');grid on;

noise=awgn(out,5,'measured') ;      %awgn���ڲ�����˹������
subplot(424); plot(t,noise);axis([0 10 -5.5 5.5]);xlabel('t');ylabel('����');title('����+�ź�'); grid on;

%��ͨ�˲���ʵ��
Fs=100;%
t=(1:1000)/Fs;%
[b,a]=ellip(4,0.1,40,[2,6]*2/Fs);%8����Բ��ͨ�˲�����0.1dbͨ�����ƣ�40db���˥����100Hz����Ƶ��
sf=filter(b,a,noise);%1
%figure(2)
subplot(425); 
plot(t,sf);
xlabel('t'); ylabel('����');
title('ͨ��BPF��Ĳ���'); 
axis([0 10 -5.5 5.5]);
grid on;

f=[f f f f f f f f f f f f f f f f f f f f];
s=sf.*f;
subplot(426); 
plot(t,s);%
xlabel('t'); ylabel('����');
title('ͨ�����������'); 
axis([0 10 -6 6]);
grid on;

Fs=100;
t=(1:1000)/Fs;
[b,a]=ellip(4,0.1,40,[2]*2/Fs);%8����Բ��ͨ�˲�����0.1dbͨ�����ƣ�40db���˥����100Hz����Ƶ��
sf=filter(b,a,s);
subplot(428); 
plot(t,sf);
xlabel('t'); ylabel('����');
title('ͨ��LPF��Ĳ���'); 
axis([0 10 -3 3]);
grid on;

%�����о�
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
xlabel('t'); ylabel('����');
title('�����о�����'); 
axis([0 10 -1 1.2]);
grid on;