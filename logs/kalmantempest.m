%function err = kalmantempest(data, gamma)
% data
%mu_fr = gamma(1);
%mu_ri = gamma(2);
%mu_in = gamma(3);
p0 = 0;%gamma(4);

mu_fr = .001;
mu_ri = .00001;
mu_in = .0000002;

g_r = 1;
g_i = .1;
mu_is = mu_in;

T = 60;

F = eye(5) + T*[(-mu_fr-mu_ri)/g_r   (mu_ri)/g_r                       0          0          0;
                (mu_ri)/g_i          (-mu_ri-mu_in-mu_is)/g_i    mu_in/g_i  mu_is/g_i  1/(T*g_i);
                 0             0                          0      0      0;
                 0             0                          0      0      0;
                 0             0                          0      0      0];
             
B = T*[mu_fr/g_r 0 0 0 0]';

%x0 = [18 18 10 10 .1]';
x0 = [data(1,4) data(1,5) data(1,6) data(1,7) p0]';

Tsteps = size(data,1);
t = (1:Tsteps)*T;
xs = zeros(Tsteps, numel(x0));
x = x0;
for ii = 1:Tsteps;
    x(3:4) = data(ii,6:7);
    x = F*x + B*data(ii,3);
    xs(ii,:) = x';
end

plot(t,xs); hold on
legend T_r T_i T_n T_s P
plot(t,data(:,3), 'b--', t,data(:,4), 'b-.');
plot(t,data(:,5), 'g--');
hold off;
pause(.1);

err = xs(100:end, 2) - data(100:end,5);

