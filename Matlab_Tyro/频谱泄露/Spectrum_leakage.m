fid_AM=fopen('D:\_DSP\DSP\Spectrum_leakage\ma.txt','r');
[f_xn,count]=fscanf(fid_AM,'%f');
fclose(fid_AM);
 
N=35;
fs=7000;
k=(0:N-1)*fs/N;
Xk=fft(f_xn);
AXk=abs(Xk(1:N));
subplot(2,1,1);
stem(k,AXk);
subplot(2,1,2);
stem(k,f_xn(1:N));


N=35;
%绘制Rn的FFT
Rn=ones(1,N);
Rk=fft(Rn,N*8);%注意多点DFT
absRk=abs(Rk(1:N));
plot(absRk);

%绘制Rn(n)频谱频移
absRk=[absRk(N/2:-1:1),absRk]
plot(absRk);

