function err = kalmantempest2(data, gamma)
% data
mu_fr = gamma(1);
mu_ri = gamma(2);
mu_iv = gamma(3);
mu_vn = gamma(4);
g_i = gamma(5);
g_v = gamma(6);

p0 = 0;%gamma(4);

%mu_fr = .001;
%mu_ri = .000001;
%mu_iv = .00001;
%mu_vn = .000001;

g_r = 1;
%g_i = .01;
%g_v = .5;
mu_vs = mu_vn;

T = 60;

F = eye(6) + T*[(-mu_fr-mu_ri)/g_r   mu_ri/g_r          0                   0          0          0;
                (mu_ri)/g_i         (-mu_ri-mu_iv)/g_i  mu_iv/g_i           0          0          1/(T*g_i);
                 0                  mu_iv/g_v          (-mu_iv-mu_vn-mu_vs)/g_v  mu_vn/g_v  mu_vs/g_v  0;
                 0                  0                         0          0          0          0;
                 0                  0                         0          0          0          0;
                 0                  0                         0          0          0          0];
             
B = T*[mu_fr/g_r 0 0 0 0 0]';

%x0 = [18 18 10 10 .1]';
x0 = [data(1,4) data(1,5) data(1,5) data(1,6) data(1,7) p0]';

Tsteps = size(data,1);
t = (1:Tsteps)*T;
xs = zeros(Tsteps, numel(x0));
x = x0;
for ii = 1:Tsteps;
    x(4:5) = data(ii,6:7);
    x = F*x + B*data(ii,3);
    xs(ii,:) = x';
end

plot(t,xs); hold on
legend T_r T_i T_v T_n T_s P
plot(t,data(:,3), 'b--', t,data(:,4), 'b-.');
plot(t,data(:,5), 'g--');
hold off;
pause(.1);

err = xs(100:end, 2) - data(100:end,5);

