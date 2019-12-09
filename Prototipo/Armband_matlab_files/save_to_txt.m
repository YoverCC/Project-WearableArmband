fid1 = fopen('sensor_1_d3_12_01.txt','w');
fprintf(fid1,'%d \r\n', sensor_01);
fclose(fid1);

fid2 = fopen('sensor_2_d3_12_01.txt','w');
fprintf(fid2,'%d \r\n', sensor_02);
fclose(fid2);