close all;clear all; clc;
% Delete Opened Ports in MATLAB
delete(instrfind)
% Create a Serial Object
%ser = serial('COM12', 'BaudRate',115200);
b = Bluetooth('Wearable_armband',1);
fopen(b);
DATA_SIZE = 8000;
sensor_01 = zeros(DATA_SIZE, 1);
sensor_02 = zeros(DATA_SIZE, 1);
index = 1;
% Open Communication Port
%fopen(ser);
fwrite(b,49,'uchar');
disp('Conexion lograda')
repetitions = 10;

% Load images
repose = imread('Relax.png');
exercise = imread('Exercise.png');
prepare = imread('Prepare_Exercise.png');
start = imread('Start_Exercise.png');
finish = imread('Finish_Exercise.png');

% Modificar este valor cada vez que se va a guardar en el día
captura = 1;

% Crea un directorio de la captura
% Fecha del dia
t = datetime('today');
day = day(t);
month = month(t);
formatSpecFolder = 'Data/Data_%d_%d/Captura%d';
str_folder = sprintf(formatSpecFolder,day,month,captura);
mkdir(str_folder)

% Start saving data
imshow(repose)
for i = 1:10    
    for index = 1:8000        
        temp = fscanf(b);    
        % Comienzo de la lectura
        if index == 1
            disp('Start reading, rest');
            imshow(repose)
        end
        % 2 segundos despues - Aviso para el movimiento
        if index == 2000
            disp('Prepare');
            imshow(prepare)
        end
        % 3 segundos despues
        if index == 3000
            disp('GO');
            imshow(start)
        end
        if index == 4000
            disp('Hold')
            imshow(exercise)
        end
        data = strsplit(temp,';');
        sensor_01(index) = str2double(data(1));
        sensor_02(index) = str2double(data(2));
        if index == 7000
            disp('Finish');
            imshow(finish)
        end
    end
    formatSpec1 = str_folder + "/sensor_1_%d.txt";
    formatSpec2 = str_folder + "/sensor_2_%d.txt";
    str1 = sprintf(formatSpec1,i);
    str2 = sprintf(formatSpec2,i);
    fid1 = fopen(str1,'w');
    fid2 = fopen(str2,'w');
    fprintf(fid1,'%d \r\n', sensor_01);
    fprintf(fid2,'%d \r\n', sensor_02);
    fclose(fid1);
    fclose(fid2);
end
fwrite(b,48,'uchar');
fclose(b);