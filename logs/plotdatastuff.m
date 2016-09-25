% data from tempdata
T = 60;

% 1 	os << this->timestamp << " ";
% 2 	os << this->Tpanna << " ";
% 3     os << this->Tfram << " ";
% 4     os << this->Tretur << " ";
% 5     os << this->Tnedervaning << " ";
% 6     os << this->TuteN << " ";
% 7     os << this->TuteS << " ";
% 8     os << this->shunt << " ";
% 9     os << this->Tmal << " ";
% 10	os << this->TframMal << " ";
% 11	os << this->shuntI << " ";
% 12	os << this->uStatic << " ";
% 13	os << this->uOutT << " ";
% 14	os << this->uProp << " ";
% 15	os << this->uInt << " ";
% 16  boiler hold flag (controller state)
% 17  control mode
% 18  pellet counter
% 19  electric counter (Wh)

fid = fopen('../logsfiles/binlogfile','r');
data = fread(fid, [19 inf], 'double')';
fclose(fid);

%t = (1:size(data,1))*T;
t = data(:,1);
tc = clock;
tt = (t-max(t))/60/60+tc(4)+tc(5)/60;

subplot(211);
plot(tt, data(:,2:10)); hold on;
%plot(((t/60/60)-29.33+20.5)/24, data(:,9)); hold off;
legend panna fram retur inne ute_{N} ute_{S} shunt  Tmal  TframMal 
%        2     3    4     5     6        7      8     9      10       
grid on

subplot(212);
plot(tt, 1-data(:,[16]));
legend boiler
%plot(((t/60/60)-29.33+20.5)/1, data(:,[8 11]));
%legend shunt shuntI
grid on
%%
plot((data(:,3)-data(:,4))./(data(:,2)-data(:,4)))

%% param ident

%gamma0 = log([.001 .000001 .00001 .000001 .01 .5]);
%gamma0 = log([.000001 .00001 .000001 .01 .001]);
gamma0 = gamma;


minfunc = @(gamma) kalmantempest3(data, exp(gamma));

gamma = lsqnonlin(minfunc, gamma0);

save newgamma gamma


%% electricity
electr = (data(:,19)-data(1,19))/1000;
plot(tt/24, electr); 
ylabel kWh

dtsigma = 5;
filtx = (-5*dtsigma:5*dtsigma)';
dtfilt = -filtx/dtsigma.^2 .* 1/sqrt(2*pi*dtsigma^2).*exp(-filtx.^2/(2*dtsigma.^2))/T*3600;
electrrate = conv(electr, dtfilt, 'same');
electrrate(electrrate < 0) = 0;
plot(tt/24, electrrate);
ylabel kW

%% pellets
kgpertick = (0.868-0.128)/7996;
pellets = (data(:,18)-data(1,18))*kgpertick;

% daywide filter
filterw = round(24*60*60/T);
pelletsd = conv(pellets, ones(filterw,1)/filterw, 'same');

subplot(211);
plot(tt/24,pellets, tt/24, pelletsd);
ylabel kg
grid on

% filter
dtsigma = 1.5;
filtx = (-5*dtsigma:5*dtsigma)';
dtfilt = -filtx/dtsigma.^2 .* 1/sqrt(2*pi*dtsigma^2).*exp(-filtx.^2/(2*dtsigma.^2))/T;
pelletsrate = conv(pellets, dtfilt, 'same');
pelletsrate(pelletsrate < 0) = 0;

dtsigma = filterw/4;
filtx = (-5*dtsigma:5*dtsigma)';
dtfilt = -filtx/dtsigma.^2 .* 1/sqrt(2*pi*dtsigma^2).*exp(-filtx.^2/(2*dtsigma.^2))/T;
pelletsrated = conv(pellets, dtfilt, 'same');
pelletsrated(pelletsrated < 0) = 0;

subplot(212);
plot(tt/24,pelletsrate*3600, tt/24,pelletsrated*3600)
ylabel kg/h
grid on
sum(pelletsrate)*T

%for ii = 2:numel(pellets)
%    if (pellets(ii-1) == pellets(ii))
%        pellets(ii:end) = pellets(ii:end)-pellets(ii);
%    end
%end

%subplot(212);
%plot(tt,pellets);
%% pellets, temp
subplot(211);
plot(tt, data(:,[6 7]));
grid on
ylabel temp

subplot(212);
invpelletsrated = 1./(3600*pelletsrated);
invpelletsrated(1) = 0; invpelletsrated(2) = NaN;
invpelletsrated(invpelletsrated > 6) = NaN;
plot( tt,16*invpelletsrated)
ylabel h/säck
grid on;




%% shunt
clf;
subplot(211);
plot(tt, data(:,[2 3 4 10])); grid on
legend Tpanna Tfram Tretur TframMal
subplot(212);
plot(tt, data(:,[8 11])); grid on
legend shunt shuntI
%plot(((t/60/60)-29.33+20.5)/1, (data(:,3)-data(:,4))./(data(:,2)-data(:,4)),'r--');
hold off;
grid on

%% shunt function ident (check with more data..)
clf;
plot(data(:,8), (data(:,3)-data(:,4))./(data(:,2)-data(:,4)),'rx'); hold on;
plot([0 1],[0 1],'b--');
axis([0 1 0 1]);

%% tempreg.
clf;
subplot(211)
plot(tt, data(:,[9 5])); grid on
legend Tmal Tnedervaning

subplot(212)
plot(tt, data(:,[10 3 12 13 14 15])); grid on
legend TframMal Tfram uStat uOutT uProp uInt


