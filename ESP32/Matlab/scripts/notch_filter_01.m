%% PROGRAM TO READ SERIAL PORT WITH ESP32
% Name: Yover Castro
% Email: ycastroc@uni.pe

close all;clear all;clc;

%First filter Notch 60Hz
wo = 60/(1000/2);
bw = wo/15;
[num, den] = iirnotch(wo,bw);

disp('Filtro Notch 60 Hz')
disp('Numerador:')
disp(num)
disp('Denominador')
disp(den)
fvtool(num,den)

%Second filter Notch 120Hz
wo2 = 120/(1000/2);
bw2 = wo2/15;
[num2, den2] = iirnotch(wo2,bw2);
disp('Filtro Notch 120 Hz')
disp('Numerador:')
disp(num2)
disp('Denominador')
disp(den2)
fvtool(num2,den2)

%Third filter High Pass 20Hz
wo3 = 20/(1000/2); %Frecuencia normalizada [0 - 1]
[num3,den3] = butter(2,wo3,'high');
disp('Filtro Butterworth pasa alta 20Hz')
disp('Numerador:')
disp(num3)
disp('Denominador')
disp(den3)
fvtool(num3,den3)

%Fourth filter Low Pass 500Hz
wo4 = 500/(1001/2); %Frecuencia normalizada [0 - 1]
[num4,den4] = butter(2,wo4,'low');
disp('Filtro Butterworth pasa baja 500Hz')
disp('Numerador:')
disp(num4)
disp('Denominador')
disp(den4)
fvtool(num4,den4)

fileID = fopen('../data/signal_emg_01.txt','r');
formatSpec = '%f';
x = fscanf(fileID,formatSpec);

%Filtrando la señal x

y = filter(num,den,x);

y2 = filter(num2,den2,y);

y3 = filter(num3,den3,y2);

y4 = filter(num4,den4,y3);

Fs = 1000;
espectro = fft(x);
L = length(x);
P2 = abs(espectro/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
figure
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')

figure
plot(x)
title('Señal sin filtro');

figure
plot(y)
title('Señal filtrada - Notch 60Hz')

figure
plot(y2)
title('Señal filtrada - Notch 120Hz')

figure
plot(y3)
title('Señal filtrada - Pasa alta 20Hz')

figure
plot(y4)
title('Señal filtrada - Pasa baja 500Hz')

Fs = 1000;
espectro = fft(y4);
L = length(y4);
P2 = abs(espectro/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
figure
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of Y4(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')