function [Aout,bout] = myProjection(Ain,bin,dim)
% [Aout,bout] = myProjection(Ain,bin,dim)
% Projection of {x: Ain*x<=bin} onto x(1:(n-dim)).

THEFOLDER = './';

TEMP = [bin,-Ain];

fid = fopen('MATLABtoLOWLEVEL','w+');
fwrite(fid, uint8(dim), 'uint8');
fwrite(fid, uint8(size(TEMP,1)), 'uint8');
fwrite(fid, uint8(size(TEMP,2)), 'uint8');
fwrite(fid, TEMP, 'double');
fclose(fid);

[status,cmdout] = system([THEFOLDER,'ProjectIt']);

fid = fopen('LOWLEVELtoMATLAB','r');
m = fread(fid,1,'uint8');
n = fread(fid,1,'uint8');
out = fread(fid, [m, n], 'double');
fclose(fid);
[status,cmdout] = system('rm MATLABtoLOWLEVEL LOWLEVELtoMATLAB TEMP');
bout = out(:,1);
Aout = -out(:,2:end);