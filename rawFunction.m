function [Aout,bout] = FUNCTION_NAME(ARGUMENT_LIST)

THEFOLDER = 'REPLACE_PATH';

TEMP = [bin,-Ain];

fid = fopen('/Volumes/MyDrive/MATLABtoLOWLEVEL','w+');
fwrite(fid, uint32(size(TEMP,1)), 'uint32');
fwrite(fid, uint32(size(TEMP,2)), 'uint32');
fwrite(fid, TEMP, 'double');
fwrite(fid, uint32(LAST_ARGUMENT), 'uint32');
fclose(fid);

[~,~] = system([THEFOLDER,'/FUNCTION_CALL'],'-echo');

fid = fopen('/Volumes/MyDrive/LOWLEVELtoMATLAB','r');
m = fread(fid,1,'uint32');
n = fread(fid,1,'uint32');
out = fread(fid, [m, n], 'double');
fclose(fid);
[~,~] = system('rm /Volumes/MyDrive/MATLABtoLOWLEVEL /Volumes/MyDrive/LOWLEVELtoMATLAB');
bout = out(:,1);
Aout = -out(:,2:end);
