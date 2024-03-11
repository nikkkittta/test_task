clear;
close all;

%====Константны====%
Fs = 4500;
a = [1 0.2 0.1];
b = [0.75 0.5];

%======Построение сигналов=========%

t = 0:.1:89.7;  %отсчёты времени
w = -pi:0.007:pi;
f = Fs*w/(2*pi);  %частота 



%===========описание сигналов==========%


signal_sin = sin(200*pi*t) + sin(25*pi*t) + sin(500*pi*t); %СИНУСОИДА

signal_pil = sawtooth(t, 1); %пилообразный сигнал

signal_meandr = square(t, 50);


%========РАСЧЁТ СПЕКТРАЛЬНЫХ ХАРАКТЕРИСТИК=========


%=========спектры синус========%
spectr_signal_sin = fftshift(signal_sin);  %спектр оригинального сигнала

noise_signal_sin = awgn(signal_sin, 0.25); %инициализация зашумлённого сигнала (оригинал + АБГШ)
    spectr_noise_sin = fftshift(noise_signal_sin); %спектр зашумлённого сигнала

signal_sin_filtred = filter(b, a, signal_sin); %фильтрация сигнала
    spectr_sin_filtred = fftshift(signal_sin_filtred); %спектр фильтрованного сигнала
 
%=========спектры пилообразный========%
spectr_signal_pil = fftshift(signal_meandr); %спектр оригинального пилообразного сигнала

noise_signal_pil = awgn(signal_pil, 0.25);
    spectr_noise_pil = fftshift(noise_signal_pil);

signal_pil_filtred = filter(b, a, signal_pil);
    spectr_pil_filtred = fftshift(signal_pil_filtred);


%=========спектры меандр========%
spectr_signal_meandr = fftshift(signal_meandr);

noise_signal_meandr = awgn(signal_meandr, 0.25);
    spectr_noise_meandr = fftshift(noise_signal_meandr);

signal_meandr_filtred = filter(b, a, signal_meandr);
    spectr_meandr_filtred = fftshift(signal_meandr_filtred);

%====построение графиков====%
figure('Name','Синусоидальный сигнал','NumberTitle','off');

subplot(2, 3, 1);
plot(t, signal_sin, LineWidth=1); grid on; xlim([0 10]); 
hold on;
plot(t, signal_sin_filtred, LineWidth=1); grid on; xlim([0 10]);
legend('Оригинал', 'Отфильтрованный сигнал');
xlabel('Время, с');
ylabel('U(s)');
title('Сигнал');

subplot(2, 3, 2);
plot(t, noise_signal_sin, LineWidth=1); grid on; xlim([0 10]);
xlabel('Время, с');
ylabel('U(s)');
title('Сигнал+Шум');

subplot(2, 3, 3);
plot(t, signal_sin_filtred, LineWidth=1); grid on; xlim([0 10]);
xlabel('Время, с');
ylabel('U(s)');
title('Выход фильтра');


subplot(2, 3, 4);
plot(f, abs(spectr_signal_sin)); 
hold on;
plot(f, abs(spectr_sin_filtred), LineWidth=1, Color= [0, 0.5, 0]); xlim([0 100]);
legend('Оригинал', 'Отфильтрованный сигнал');
xlabel('Частота, кГц');

title('Амплитудный спектр сигнала');

subplot(2, 3, 5);
plot(t, spectr_noise_sin, LineWidth=1); grid on; xlim([0 10]);
xlabel('Частота, кГц');
title('Спектр зашумлённого сигнала');


subplot(2, 3, 6);
plot(f, abs(spectr_sin_filtred)); xlim([0 100]);
xlabel('Частота, кГц');
title('Амплитудный спектр фильрованного сигнала');


%======второе окно=======%
figure('Name','Пилообразный сигнал','NumberTitle','off');


subplot(2, 3, 1);
plot(t, signal_pil); grid on;
hold on;
plot(t, signal_pil_filtred, LineWidth=1); grid on; xlim([0 10]);
legend('Оригинал', 'Отфильтрованный сигнал');
xlabel('Время, с');
ylabel('U(s)');
title('Пилообразный импульс');

subplot(2, 3, 2);
plot(t, noise_signal_pil, LineWidth=1); grid on; xlim([0 10]);
xlabel('Время, с');
ylabel('U(s)');
title('Сигнал+Шум');

subplot(2, 3, 3);
plot(t, signal_pil_filtred, LineWidth=1); grid on; xlim([0 10]);
xlabel('Время, с');
ylabel('U(s)');
title('Выход фильтра');


subplot(2, 3, 4);
plot(f, abs(spectr_signal_pil)); 
hold on;
plot(f, abs(spectr_pil_filtred), LineWidth=1, Color= [0, 0.5, 0]); xlim([0 100]);
legend('Оригинал', 'Отфильтрованный сигнал');
xlabel('Частота, кГц');

title('Амплитудный спектр сигнала');

subplot(2, 3, 5);
plot(t, spectr_noise_pil, LineWidth=1); grid on; xlim([0 10]);
xlabel('Частота, кГц');
title('Спектр зашумлённого сигнала');


subplot(2, 3, 6);
plot(f, abs(spectr_pil_filtred)); xlim([0 100]);
xlabel('Частота, кГц');
title('Амплитудный спектр фильрованного сигнала');



%======Маендр окно=======%
figure('Name','Меандр','NumberTitle','off');


subplot(2, 3, 1);
plot(t, signal_meandr); grid on;
hold on;
plot(t, signal_meandr_filtred, LineWidth=1); grid on; xlim([0 10]);
legend('Оригинал', 'Отфильтрованный сигнал');
xlabel('Время, с');
ylabel('U(s)');
title('Пилообразный импульс');

subplot(2, 3, 2);
plot(t, noise_signal_meandr, LineWidth=1); grid on; xlim([0 10]);
xlabel('Время, с');
ylabel('U(s)');
title('Сигнал+Шум');

subplot(2, 3, 3);
plot(t, signal_meandr_filtred, LineWidth=1); grid on; xlim([0 10]);
xlabel('Время, с');
ylabel('U(s)');
title('Выход фильтра');


subplot(2, 3, 4);
plot(f, abs(spectr_signal_meandr)); 
hold on;
plot(f, abs(spectr_meandr_filtred), LineWidth=1, Color= [0, 0.5, 0]); xlim([0 100]);
legend('Оригинал', 'Отфильтрованный сигнал');
xlabel('Частота, кГц');

title('Амплитудный спектр сигнала');

subplot(2, 3, 5);
plot(t, spectr_noise_meandr, LineWidth=1); grid on; xlim([0 10]);
xlabel('Частота, кГц');
title('Спектр зашумлённого сигнала');


subplot(2, 3, 6);
plot(f, abs(spectr_meandr_filtred)); xlim([0 100]);
xlabel('Частота, кГц');
title('Амплитудный спектр фильрованного сигнала');
